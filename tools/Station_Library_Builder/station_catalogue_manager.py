#!/usr/bin/env python3

"""
Station Catalogue Manager
Internet Radio Project

Downloads, audits and builds a curated UK station library.

Version: 1.0.2
"""

from __future__ import annotations

import argparse
import csv
import json
import re
import shutil
import sys
import urllib.error
import urllib.parse
import urllib.request
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from logo_builder import build_default_logo, build_station_logo
from station_library import write_station_library
from bbc_builder import build_bbc_station_library
from catalogue_builder import write_filter_catalogues
from common import CATALOGUES_DIR, CATALOGUE_INDEX_FILE
from report_builder import write_build_report
from sync_assets import sync_firmware_assets

APP_NAME = "InternetRadioProject-StationCatalogueManager"
VERSION = "1.0.2"
BUILD_TIME = datetime.now(timezone.utc)
CATALOGUE_VERSION = BUILD_TIME.strftime("%Y-%m-%d")
BUILD_TIMESTAMP = BUILD_TIME.isoformat(timespec="seconds").replace("+00:00", "Z")

BASE_URL = "https://de1.api.radio-browser.info/json/stations/search"
COUNTRY_CODE = "GB"
PAGE_SIZE = 1000
REQUEST_TIMEOUT_SECONDS = 30

TOOL_DIR = Path(__file__).resolve().parent
OUTPUT_DIR = TOOL_DIR / "output"

RAW_CATALOGUE_FILE = OUTPUT_DIR / "gb_catalogue.json"
CURATED_STATIONS_FILE = TOOL_DIR / "curated_stations.json"
AUDIT_REPORT_FILE = OUTPUT_DIR / "audit_report.txt"
DUPLICATE_NAMES_FILE = OUTPUT_DIR / "duplicate_names.csv"
DUPLICATE_URLS_FILE = OUTPUT_DIR / "duplicate_urls.csv"
CANDIDATE_REPORT_FILE = OUTPUT_DIR / "candidate_stations_report.txt"
CANDIDATE_CSV_FILE = OUTPUT_DIR / "candidate_stations.csv"
STATION_LIBRARY_FILE = OUTPUT_DIR / "station_library.json"
BUILD_REPORT_FILE = OUTPUT_DIR / "station_library_report.txt"


Station = dict[str, Any]


def load_catalogue() -> list[Station]:
    """Load the previously downloaded GB catalogue."""

    if not RAW_CATALOGUE_FILE.exists():
        raise RuntimeError(
            "No downloaded catalogue was found. Run the audit command first."
        )

    try:
        with RAW_CATALOGUE_FILE.open("r", encoding="utf-8") as file:
            stations = json.load(file)
    except json.JSONDecodeError as error:
        raise RuntimeError("The downloaded catalogue contains invalid JSON") from error

    if not isinstance(stations, list):
        raise RuntimeError("The downloaded catalogue has an unexpected format")

    return stations


def load_curated_manifest() -> dict[str, Any]:
    """Load and validate the maintained station manifest."""

    if not CURATED_STATIONS_FILE.exists():
        raise RuntimeError(
            f"Curated station manifest not found: {CURATED_STATIONS_FILE}"
        )

    try:
        with CURATED_STATIONS_FILE.open("r", encoding="utf-8") as file:
            manifest = json.load(file)
    except json.JSONDecodeError as error:
        raise RuntimeError(
            "The curated station manifest contains invalid JSON"
        ) from error

    if not isinstance(manifest, dict):
        raise RuntimeError("The curated station manifest must contain a JSON object")

    stations = manifest.get("stations")

    if not isinstance(stations, list):
        raise RuntimeError("The curated station manifest must contain a stations list")

    return manifest


def stream_score(station: Station) -> tuple[int, list[str]]:
    """Return an explainable suitability score for a stream record."""

    score = 0
    reasons: list[str] = []

    stream_url = preferred_url(station)
    parsed_url = urllib.parse.urlsplit(stream_url)
    codec = str(station.get("codec") or "UNKNOWN").upper()
    bitrate = int(station.get("bitrate") or 0)

    if parsed_url.scheme == "https":
        score += 40
        reasons.append("+40 HTTPS")
    elif parsed_url.scheme == "http":
        score += 20
        reasons.append("+20 HTTP")
    else:
        score -= 40
        reasons.append("-40 unsupported URL scheme")

    if not parsed_url.query:
        score += 20
        reasons.append("+20 clean URL")
    else:
        score -= 5
        reasons.append("-5 query-string URL")

    if codec == "MP3":
        score += 35
        reasons.append("+35 MP3")
    elif codec in {"AAC", "AAC+"}:
        score += 20
        reasons.append(f"+20 {codec}")
    elif codec == "OGG":
        score += 5
        reasons.append("+5 OGG")
    else:
        score -= 50
        reasons.append(f"-50 unsupported codec ({codec})")

    if 64 <= bitrate <= 192:
        score += 20
        reasons.append(f"+20 suitable bitrate ({bitrate} kbps)")
    elif bitrate in {48, 256}:
        score += 10
        reasons.append(f"+10 usable bitrate ({bitrate} kbps)")
    elif bitrate == 0:
        reasons.append("+0 unknown bitrate")
    else:
        score -= 5
        reasons.append(f"-5 less suitable bitrate ({bitrate} kbps)")

    if station.get("lastcheckok") in (1, True, "1"):
        score += 15
        reasons.append("+15 last check OK")
    else:
        score -= 50
        reasons.append("-50 last check failed")

    if station.get("ssl_error") in (1, True, "1"):
        score -= 25
        reasons.append("-25 SSL error")

    if is_hls_station(station):
        score -= 10
        reasons.append("-10 HLS")

    if is_likely_video(station):
        score -= 100
        reasons.append("-100 likely video")

    votes = int(station.get("votes") or 0)
    clickcount = int(station.get("clickcount") or 0)

    popularity_bonus = min(votes // 100, 10) + min(clickcount // 10, 10)
    if popularity_bonus:
        score += popularity_bonus
        reasons.append(f"+{popularity_bonus} popularity")

    return score, reasons


def select_preferred_candidates(
    duplicate_names: dict[str, list[Station]],
) -> list[tuple[str, Station, int, list[str], list[tuple[Station, int, list[str]]]]]:
    """Rank duplicate-name groups and select one preferred stream."""

    selections = []

    for normalised_name, records in sorted(duplicate_names.items()):
        ranked = []

        for station in records:
            score, reasons = stream_score(station)
            ranked.append((station, score, reasons))

        ranked.sort(
            key=lambda item: (
                -item[1],
                -int(item[0].get("votes") or 0),
                preferred_url(item[0]),
            )
        )

        selected_station, selected_score, selected_reasons = ranked[0]
        alternatives = ranked[1:]

        selections.append(
            (
                normalised_name,
                selected_station,
                selected_score,
                selected_reasons,
                alternatives,
            )
        )

    return selections


def write_candidate_csv(
    selections: list[
        tuple[
            str,
            Station,
            int,
            list[str],
            list[tuple[Station, int, list[str]]],
        ]
    ],
) -> None:
    """Write the preferred duplicate-name candidates to CSV."""

    with CANDIDATE_CSV_FILE.open(
        "w",
        encoding="utf-8",
        newline="",
    ) as file:
        writer = csv.writer(file)

        writer.writerow(
            [
                "normalised_name",
                "station_name",
                "score",
                "codec",
                "bitrate",
                "stream_url",
                "homepage",
                "favicon",
                "station_uuid",
                "reasons",
                "alternative_count",
            ]
        )

        for (
            normalised_name,
            station,
            score,
            reasons,
            alternatives,
        ) in selections:
            writer.writerow(
                [
                    normalised_name,
                    station_name(station),
                    score,
                    station.get("codec") or "",
                    station.get("bitrate") or 0,
                    preferred_url(station),
                    station.get("homepage") or "",
                    station.get("favicon") or "",
                    station.get("stationuuid") or "",
                    "; ".join(reasons),
                    len(alternatives),
                ]
            )


def write_candidate_report(
    selections: list[
        tuple[
            str,
            Station,
            int,
            list[str],
            list[tuple[Station, int, list[str]]],
        ]
    ],
) -> None:
    """Write a readable candidate selection report."""

    lines = [
        "Station Catalogue Manager",
        "Candidate Stations Report",
        "=========================",
        "",
        f"Manager version: {VERSION}",
        f"Duplicate station groups: {len(selections)}",
        "",
    ]

    for (
        normalised_name,
        selected,
        selected_score,
        selected_reasons,
        alternatives,
    ) in selections:
        lines.extend(
            [
                normalised_name,
                "-" * len(normalised_name),
                "SELECTED",
                f"Name:    {station_name(selected)}",
                f"Score:   {selected_score}",
                f"Codec:   {selected.get('codec') or 'UNKNOWN'}",
                f"Bitrate: {selected.get('bitrate') or 0} kbps",
                f"URL:     {preferred_url(selected)}",
                "Reasons:",
            ]
        )

        lines.extend(f"  {reason}" for reason in selected_reasons)

        if alternatives:
            lines.append("")
            lines.append("ALTERNATIVES")

            for station, score, reasons in alternatives:
                lines.extend(
                    [
                        f"- {station_name(station)}",
                        f"  Score:   {score}",
                        f"  Codec:   {station.get('codec') or 'UNKNOWN'}",
                        f"  Bitrate: {station.get('bitrate') or 0} kbps",
                        f"  URL:     {preferred_url(station)}",
                        f"  Reasons: {'; '.join(reasons)}",
                    ]
                )

        lines.extend(["", ""])

    CANDIDATE_REPORT_FILE.write_text(
        "\n".join(lines),
        encoding="utf-8",
    )


def run_candidates() -> None:
    """Select preferred streams for exact duplicate station names."""

    ensure_output_directory()

    print("Station Catalogue Manager")
    print(f"Version {VERSION}")
    print()

    stations = load_catalogue()

    non_bbc = [station for station in stations if not is_bbc_station(station)]

    duplicate_names, _ = build_duplicate_groups(non_bbc)
    selections = select_preferred_candidates(duplicate_names)

    write_candidate_csv(selections)
    write_candidate_report(selections)

    print(f"Loaded catalogue records: {len(stations)}")
    print(f"Non-BBC candidates:       {len(non_bbc)}")
    print(f"Duplicate-name groups:    {len(selections)}")
    print()
    print("Output files")
    print("------------")
    print(CANDIDATE_CSV_FILE)
    print(CANDIDATE_REPORT_FILE)


def clean_display_name(name: str) -> str:
    """Return a conservative listener-facing station name."""

    cleaned = re.sub(r"\s+", " ", name).strip(" ._-")

    # Remove obvious slogan text after a colon.
    if ":" in cleaned:
        prefix, suffix = cleaned.split(":", 1)
        if len(prefix.strip()) >= 3 and len(suffix.strip()) >= 8:
            cleaned = prefix.strip()

    # Remove obvious promotional suffixes beginning with " - The".
    cleaned = re.sub(
        r"\s+-\s+The\b.*$",
        "",
        cleaned,
        flags=re.IGNORECASE,
    )

    return cleaned.strip() or name.strip()


def is_supported_library_record(station: Station) -> tuple[bool, str]:
    """Return whether a record is suitable for the first production library."""

    if is_bbc_station(station):
        return False, "BBC handled separately"

    if is_likely_video(station):
        return False, "likely video"

    if is_hls_station(station):
        return False, "HLS excluded from Radio Browser library"

    codec = str(station.get("codec") or "").upper()
    if codec not in {"MP3", "AAC", "AAC+"}:
        return False, f"unsupported codec ({codec or 'UNKNOWN'})"

    if not preferred_url(station):
        return False, "missing stream URL"

    if station.get("lastcheckok") not in (1, True, "1"):
        return False, "last check failed"

    return True, ""


def canonical_stream_url(station: Station) -> str:
    """Return a stable URL key for duplicate detection."""

    stream_url = preferred_url(station).strip()
    parsed = urllib.parse.urlsplit(stream_url)

    # Remove fragments because they are not sent to the stream server.
    return urllib.parse.urlunsplit(
        (
            parsed.scheme.casefold(),
            parsed.netloc.casefold(),
            parsed.path.rstrip("/") or "/",
            parsed.query,
            "",
        )
    )


def build_station_library(
    stations: list[Station],
) -> tuple[list[dict[str, Any]], Counter[str]]:
    """Build the production library from the curated station manifest."""

    manifest = load_curated_manifest()
    definitions = manifest["stations"]

    counts: Counter[str] = Counter()
    library: list[dict[str, Any]] = []
    missing_required: list[str] = []

    counts["manifest definitions"] = len(definitions)

    for definition in definitions:
        station_id = str(definition.get("id") or "").strip()
        display_name = str(definition.get("name") or "").strip()
        aliases = definition.get("aliases")
        filters = definition.get("filters")
        required = definition.get("required")

        if not station_id:
            raise RuntimeError(
                f"Manifest station is missing an id: " f"{display_name or '<unnamed>'}"
            )

        if not display_name:
            raise RuntimeError(f"Manifest station {station_id} is missing a name")

        if not isinstance(aliases, list) or not aliases:
            raise RuntimeError(f"Manifest station {station_id} must contain aliases")

        if not isinstance(filters, list):
            raise RuntimeError(
                f"Manifest station {station_id} must contain a filters list"
            )

        if not isinstance(required, bool):
            raise RuntimeError(
                f"Manifest station {station_id} must contain required true/false"
            )

        alias_keys = {
            normalise_name(str(alias)) for alias in aliases if str(alias).strip()
        }

        raw_matches = [
            station
            for station in stations
            if normalise_name(station_name(station)) in alias_keys
        ]

        supported_matches = [
            station
            for station in raw_matches
            if is_supported_library_record(station)[0]
        ]

        if not supported_matches:
            if raw_matches:
                counts["manifest matches unsupported"] += 1
            else:
                counts["manifest stations not found"] += 1

            if required:
                missing_required.append(display_name)
                counts["required stations missing"] += 1
            else:
                counts["optional stations omitted"] += 1

            continue

        ranked = sorted(
            supported_matches,
            key=lambda station: (
                -stream_score(station)[0],
                -int(station.get("votes") or 0),
                preferred_url(station),
            ),
        )

        selected = ranked[0]

        library.append(
            {
                "id": station_id,
                "name": display_name,
                "filters": [
                    str(filter_name).strip().casefold()
                    for filter_name in filters
                    if str(filter_name).strip()
                ],
                "url": preferred_url(selected),
                "codec": str(selected.get("codec") or "").upper(),
                "bitrate": int(selected.get("bitrate") or 0),
                "homepage": str(selected.get("homepage") or "").strip(),
                "favicon": str(selected.get("favicon") or "").strip(),
                "state": str(selected.get("state") or "").strip(),
                "tags": str(selected.get("tags") or "").strip(),
                "stationuuid": str(selected.get("stationuuid") or "").strip(),
            }
        )

        counts["curated stations resolved"] += 1

    if missing_required:
        raise RuntimeError(
            "Required curated stations could not be built: "
            + ", ".join(missing_required)
        )

    library.sort(key=lambda station: str(station.get("name") or "").casefold())

    counts["eligible supported records"] = len(library)
    counts["unique normalised names"] = len(library)
    counts["final unique stations"] = len(library)

    return library, counts


def build_local_station_logos(
    libraries: list[list[dict[str, Any]]],
) -> int:
    """Build local PNG logos and write an artwork curation report."""

    logos_dir = OUTPUT_DIR / "logos"
    report_path = OUTPUT_DIR / "artwork_curation_report.txt"

    if logos_dir.exists():
        shutil.rmtree(logos_dir)

    logos_dir.mkdir(
        parents=True,
        exist_ok=True,
    )

    default_logo = build_default_logo(logos_dir)

    processed_ids: set[str] = set()

    report_groups: dict[str, list[str]] = {
        "No artwork available": [],
        "Download failed": [],
        "Invalid image": [],
        "Source image too small": [],
        "Other": [],
    }

    built_count = 0
    downloaded_count = 0
    curated_count = 0
    default_count = 0

    def add_report_entry(
        station_name: str,
        station_id: str,
        reason: str,
    ) -> None:
        """Add one station to the appropriate report group."""

        entry = (
            f"{station_name}\n"
            f"  ID: {station_id}\n"
            f"  Reason: {reason}\n"
        )

        reason_lower = reason.casefold()

        if reason == "No favicon URL":
            report_groups["No artwork available"].append(entry)

        elif "unable to download logo" in reason_lower:
            report_groups["Download failed"].append(entry)

        elif "not a supported image" in reason_lower:
            report_groups["Invalid image"].append(entry)

        elif "source image too small" in reason_lower:
            report_groups["Source image too small"].append(entry)

        else:
            report_groups["Other"].append(entry)

    for library in libraries:
        for station in library:
            station_id = str(station.get("id") or "").strip()
            station_name = str(station.get("name") or station_id).strip()

            if not station_id or station_id in processed_ids:
                continue

            processed_ids.add(station_id)

            try:
                result = build_station_logo(
                    station,
                    logos_dir,
                )

                local_logo = result.path

                if result.status == "curated":
                    curated_count += 1
                    print(f"Logo built: {station_name} (curated)")

                elif result.status == "downloaded":
                    downloaded_count += 1
                    print(f"Logo built: {station_name} (downloaded)")

                else:
                    default_count += 1

                    add_report_entry(
                        station_name,
                        station_id,
                        result.detail,
                    )

                    print(
                        f"Default logo: {station_name}"
                        f" — {result.detail}"
                    )

            except RuntimeError as error:
                local_logo = default_logo
                default_count += 1

                reason = str(error)

                add_report_entry(
                    station_name,
                    station_id,
                    reason,
                )

                print(
                    f"Warning: logo failed for "
                    f"{station_name}: {reason}"
                )

            station["logo"] = local_logo
            built_count += 1

    report_lines = [
        "Station Artwork Curation Report",
        "===============================",
        "",
        f"Stations processed: {built_count}",
        f"Downloaded artwork: {downloaded_count}",
        f"Curated artwork:    {curated_count}",
        f"Default artwork:    {default_count}",
        "",
        "Stations requiring artwork curation",
        "------------------------------------",
        "",
    ]

    for group_name, entries in report_groups.items():
        if not entries:
            continue

        report_lines.extend(
            [
                group_name,
                "-" * len(group_name),
                "",
            ]
        )

        report_lines.extend(entries)

    if default_count == 0:
        report_lines.append("None")

    report_path.write_text(
        "\n".join(report_lines),
        encoding="utf-8",
    )

    print()
    print(f"Artwork curation report: {report_path}")

    return built_count


def run_build() -> None:
    """Build the production UK station library."""

    ensure_output_directory()

    print("Station Catalogue Manager")
    print(f"Version {VERSION}")
    print()

    stations = load_catalogue()
    library, counts = build_station_library(stations)
    bbc_stations = build_bbc_station_library()

    logo_count = build_local_station_logos(
        [
            library,
            bbc_stations,
        ]
    )

    write_station_library(library)

    filter_index = write_filter_catalogues(
        library,
        bbc_stations,
    )

    print(f"Local station logos: {logo_count}")

    write_build_report(
        len(stations),
        library,
        counts,
    )

    data_library_file, data_catalogues_dir, data_logos_dir = sync_firmware_assets()

    print(f"Loaded catalogue records: {len(stations)}")
    print(f"Final unique stations:    {len(library)}")
    print()
    print("Filter catalogues")
    print("-----------------")

    for filter_name, details in filter_index.items():
        print(
            f"{details['label']:<10}"
            f"{details['station_count']:>5} stations  "
            f"{details['page_count']:>3} pages"
        )

    print()
    print("Output files")
    print("------------")
    print(STATION_LIBRARY_FILE)
    print(CATALOGUE_INDEX_FILE)
    print(CATALOGUES_DIR)
    print(BUILD_REPORT_FILE)
    print()
    print("Firmware assets (LittleFS)")
    print("--------------------------")
    print(data_library_file)
    print(data_catalogues_dir)
    print(data_logos_dir)
    print(f"Logos: {logo_count}")
    print()
    print("✓ Ready for LittleFS upload.")


def ensure_output_directory() -> None:
    """Create the output directory if it does not exist."""

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)


def normalise_name(name: str) -> str:
    """Return a consistent station name for comparison."""

    normalised = name.casefold().strip()
    normalised = re.sub(r"\s+", " ", normalised)
    return normalised


def preferred_url(station: Station) -> str:
    """Return the resolved stream URL when available."""

    resolved_url = str(station.get("url_resolved") or "").strip()
    original_url = str(station.get("url") or "").strip()

    return resolved_url or original_url


def station_name(station: Station) -> str:
    """Return a safely trimmed station name."""

    return str(station.get("name") or "").strip()


def is_bbc_station(station: Station) -> bool:
    """Identify Radio Browser records labelled as BBC."""

    return "bbc" in station_name(station).casefold()


def is_hls_station(station: Station) -> bool:
    """Identify streams marked as HLS or using an HLS playlist URL."""

    hls_value = station.get("hls")
    stream_url = preferred_url(station).casefold()

    return hls_value in (1, True, "1") or ".m3u8" in stream_url


def is_likely_video(station: Station) -> bool:
    """Identify records that appear to be television or video streams."""

    name = station_name(station).casefold()
    codec = str(station.get("codec") or "").casefold()

    video_name_terms = (
        " tv",
        "television",
        "video",
        "news hd",
    )

    return any(term in name for term in video_name_terms) or "h.264" in codec


def download_gb_catalogue() -> list[Station]:
    """Download every available GB station record using pagination."""

    stations: list[Station] = []
    offset = 0

    while True:
        query = urllib.parse.urlencode(
            {
                "countrycode": COUNTRY_CODE,
                "hidebroken": "true",
                "order": "stationuuid",
                "limit": PAGE_SIZE,
                "offset": offset,
            }
        )

        request = urllib.request.Request(
            f"{BASE_URL}?{query}",
            headers={
                "User-Agent": f"{APP_NAME}/{VERSION}",
                "Accept": "application/json",
            },
        )

        try:
            with urllib.request.urlopen(
                request,
                timeout=REQUEST_TIMEOUT_SECONDS,
            ) as response:
                page = json.load(response)

        except urllib.error.HTTPError as error:
            raise RuntimeError(f"Radio Browser returned HTTP {error.code}") from error

        except urllib.error.URLError as error:
            raise RuntimeError(
                f"Unable to contact Radio Browser: {error.reason}"
            ) from error

        except json.JSONDecodeError as error:
            raise RuntimeError("Radio Browser returned invalid JSON") from error

        if not isinstance(page, list):
            raise RuntimeError("Radio Browser returned an unexpected response")

        page_count = len(page)

        print(f"Downloaded offset {offset:>5}: " f"{page_count:>4} records")

        stations.extend(page)

        if page_count < PAGE_SIZE:
            break

        offset += PAGE_SIZE

    return stations


def save_raw_catalogue(stations: list[Station]) -> None:
    """Save the complete downloaded catalogue as formatted JSON."""

    with RAW_CATALOGUE_FILE.open("w", encoding="utf-8") as file:
        json.dump(
            stations,
            file,
            ensure_ascii=False,
            indent=2,
        )


def build_duplicate_groups(
    stations: list[Station],
) -> tuple[dict[str, list[Station]], dict[str, list[Station]]]:
    """Group duplicate records by normalised name and stream URL."""

    names: dict[str, list[Station]] = {}
    urls: dict[str, list[Station]] = {}

    for station in stations:
        name_key = normalise_name(station_name(station))
        url_key = preferred_url(station)

        if name_key:
            names.setdefault(name_key, []).append(station)

        if url_key:
            urls.setdefault(url_key, []).append(station)

    duplicate_names = {
        key: records for key, records in names.items() if len(records) > 1
    }

    duplicate_urls = {key: records for key, records in urls.items() if len(records) > 1}

    return duplicate_names, duplicate_urls


def write_duplicate_names_csv(
    duplicate_names: dict[str, list[Station]],
) -> None:
    """Write duplicate station-name groups to CSV."""

    with DUPLICATE_NAMES_FILE.open(
        "w",
        encoding="utf-8",
        newline="",
    ) as file:
        writer = csv.writer(file)

        writer.writerow(
            [
                "normalised_name",
                "duplicate_count",
                "station_name",
                "codec",
                "bitrate",
                "stream_url",
                "station_uuid",
            ]
        )

        sorted_groups = sorted(
            duplicate_names.items(),
            key=lambda item: (-len(item[1]), item[0]),
        )

        for normalised, records in sorted_groups:
            for station in records:
                writer.writerow(
                    [
                        normalised,
                        len(records),
                        station_name(station),
                        station.get("codec") or "",
                        station.get("bitrate") or 0,
                        preferred_url(station),
                        station.get("stationuuid") or "",
                    ]
                )


def write_duplicate_urls_csv(
    duplicate_urls: dict[str, list[Station]],
) -> None:
    """Write duplicate stream-URL groups to CSV."""

    with DUPLICATE_URLS_FILE.open(
        "w",
        encoding="utf-8",
        newline="",
    ) as file:
        writer = csv.writer(file)

        writer.writerow(
            [
                "stream_url",
                "duplicate_count",
                "station_name",
                "codec",
                "bitrate",
                "station_uuid",
            ]
        )

        sorted_groups = sorted(
            duplicate_urls.items(),
            key=lambda item: (-len(item[1]), item[0]),
        )

        for stream_url, records in sorted_groups:
            for station in records:
                writer.writerow(
                    [
                        stream_url,
                        len(records),
                        station_name(station),
                        station.get("codec") or "",
                        station.get("bitrate") or 0,
                        station.get("stationuuid") or "",
                    ]
                )


def generate_audit_report(stations: list[Station]) -> str:
    """Analyse the catalogue and return a text report."""

    bbc_stations = [station for station in stations if is_bbc_station(station)]

    non_bbc = [station for station in stations if not is_bbc_station(station)]

    names = Counter(normalise_name(station_name(station)) for station in non_bbc)

    urls = Counter(preferred_url(station) for station in non_bbc)

    codecs = Counter(
        str(station.get("codec") or "UNKNOWN").upper() for station in non_bbc
    )

    bitrates = Counter(int(station.get("bitrate") or 0) for station in non_bbc)

    duplicate_name_records = sum(count - 1 for count in names.values() if count > 1)

    duplicate_url_records = sum(count - 1 for count in urls.values() if count > 1)

    missing_names = sum(not station_name(station) for station in non_bbc)

    missing_urls = sum(not preferred_url(station) for station in non_bbc)

    missing_favicons = sum(
        not str(station.get("favicon") or "").strip() for station in non_bbc
    )

    hls_count = sum(is_hls_station(station) for station in non_bbc)

    likely_video_count = sum(is_likely_video(station) for station in non_bbc)

    generated_at = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC")

    lines: list[str] = [
        "Station Catalogue Manager",
        "Radio Browser GB Catalogue Audit",
        "================================",
        "",
        f"Manager version:           {VERSION}",
        f"Generated:                 {generated_at}",
        "",
        f"Total GB records:          {len(stations)}",
        f"BBC-labelled records:      {len(bbc_stations)}",
        f"Non-BBC candidates:        {len(non_bbc)}",
        "",
        f"Unique normalised names:   {len(names)}",
        f"Duplicate-name records:    {duplicate_name_records}",
        f"Unique resolved URLs:      {len(urls)}",
        f"Duplicate-URL records:     {duplicate_url_records}",
        "",
        f"Missing names:             {missing_names}",
        f"Missing stream URLs:       {missing_urls}",
        f"Missing favicons:          {missing_favicons}",
        f"HLS streams:               {hls_count}",
        f"Likely video entries:      {likely_video_count}",
        f"Zero/unknown bitrate:      {bitrates.get(0, 0)}",
        "",
        "Codecs",
        "------",
    ]

    for codec, count in codecs.most_common():
        lines.append(f"{codec:<16}{count:>6}")

    lines.extend(
        [
            "",
            "Most Common Bitrates",
            "--------------------",
        ]
    )

    for bitrate, count in bitrates.most_common(15):
        label = "Unknown" if bitrate == 0 else f"{bitrate} kbps"

        lines.append(f"{label:<16}{count:>6}")

    lines.extend(
        [
            "",
            "Most Duplicated Station Names",
            "-----------------------------",
        ]
    )

    duplicated_names = [(name, count) for name, count in names.items() if count > 1]

    for name, count in sorted(
        duplicated_names,
        key=lambda item: (-item[1], item[0]),
    )[:30]:
        lines.append(f"{count:>4}  {name}")

    return "\n".join(lines) + "\n"


def write_audit_report(report: str) -> None:
    """Save the text audit report."""

    AUDIT_REPORT_FILE.write_text(
        report,
        encoding="utf-8",
    )


def run_audit() -> None:
    """Download the catalogue and create all v0.1 reports."""

    ensure_output_directory()

    print("Station Catalogue Manager")
    print(f"Version {VERSION}")
    print()

    stations = download_gb_catalogue()

    print()
    print("Writing catalogue and reports...")

    save_raw_catalogue(stations)

    non_bbc = [station for station in stations if not is_bbc_station(station)]

    duplicate_names, duplicate_urls = build_duplicate_groups(non_bbc)

    write_duplicate_names_csv(duplicate_names)
    write_duplicate_urls_csv(duplicate_urls)

    report = generate_audit_report(stations)
    write_audit_report(report)

    print()
    print(report)

    print("Output files")
    print("------------")
    print(RAW_CATALOGUE_FILE)
    print(AUDIT_REPORT_FILE)
    print(DUPLICATE_NAMES_FILE)
    print(DUPLICATE_URLS_FILE)


def run_manifest() -> None:
    """Validate curated station definitions against the downloaded catalogue."""

    manifest = load_curated_manifest()
    catalogue = load_catalogue()

    definitions = manifest["stations"]
    missing_required: list[str] = []

    print("Station Catalogue Manager")
    print(f"Version {VERSION}")
    print()
    print("Curated Manifest Check")
    print("----------------------")

    for definition in definitions:
        station_id = str(definition.get("id") or "").strip()
        display_name = str(definition.get("name") or "").strip()
        aliases = definition.get("aliases")
        filters = definition.get("filters")
        required = definition.get("required")

        if not station_id:
            raise RuntimeError(
                f"Manifest station is missing an id: {display_name or '<unnamed>'}"
            )

        if not display_name:
            raise RuntimeError(f"Manifest station {station_id} is missing a name")

        if not isinstance(aliases, list) or not aliases:
            raise RuntimeError(f"Manifest station {station_id} must contain aliases")

        if not isinstance(filters, list):
            raise RuntimeError(
                f"Manifest station {station_id} must contain a filters list"
            )

        if not isinstance(required, bool):
            raise RuntimeError(
                f"Manifest station {station_id} must contain required true/false"
            )

        alias_keys = {
            normalise_name(str(alias)) for alias in aliases if str(alias).strip()
        }

        raw_matches = [
            station
            for station in catalogue
            if normalise_name(station_name(station)) in alias_keys
        ]

        matches = [
            station
            for station in raw_matches
            if is_supported_library_record(station)[0]
        ]

        if not matches:
            if raw_matches:
                status = "MISSING REQUIRED (unsupported)" if required else "unsupported"
            else:
                status = "MISSING REQUIRED" if required else "not found"

            print(f"{display_name:<28} {status}")

            if required:
                missing_required.append(display_name)

            continue

        ranked = sorted(
            matches,
            key=lambda station: (
                -stream_score(station)[0],
                -int(station.get("votes") or 0),
                preferred_url(station),
            ),
        )

        selected = ranked[0]
        score, _ = stream_score(selected)

        print(
            f"{display_name:<28} "
            f"OK  {selected.get('codec') or 'UNKNOWN':<5} "
            f"{int(selected.get('bitrate') or 0):>3} kbps  "
            f"score {score}"
        )

    print()
    print(f"Manifest stations: {len(definitions)}")
    print(f"Missing required:  {len(missing_required)}")

    if missing_required:
        raise RuntimeError(
            "Required curated stations were not found: " + ", ".join(missing_required)
        )


def build_argument_parser() -> argparse.ArgumentParser:
    """Create the command-line argument parser."""

    parser = argparse.ArgumentParser(
        description=("Audit and curate UK internet-radio station catalogues.")
    )

    parser.add_argument(
        "--version",
        action="version",
        version=f"%(prog)s {VERSION}",
    )

    subparsers = parser.add_subparsers(
        dest="command",
        required=True,
    )

    subparsers.add_parser(
        "audit",
        help="Download and audit the Radio Browser GB catalogue.",
    )

    subparsers.add_parser(
        "candidates",
        help="Recommend one preferred stream for duplicate station names.",
    )

    subparsers.add_parser(
        "manifest",
        help="Validate the curated station manifest.",
    )

    subparsers.add_parser(
        "build",
        help="Build the production UK station library.",
    )

    return parser


def main() -> int:
    """Application entry point."""

    parser = build_argument_parser()
    arguments = parser.parse_args()

    try:
        if arguments.command == "audit":
            run_audit()
            return 0

        if arguments.command == "candidates":
            run_candidates()
            return 0

        if arguments.command == "manifest":
            run_manifest()
            return 0

        if arguments.command == "build":
            run_build()
            return 0

        parser.error(f"Unsupported command: {arguments.command}")

    except KeyboardInterrupt:
        print("\nCancelled.", file=sys.stderr)
        return 130

    except RuntimeError as error:
        print(f"\nError: {error}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
