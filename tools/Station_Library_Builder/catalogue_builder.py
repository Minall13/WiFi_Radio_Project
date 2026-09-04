# =====================================================
# Catalogue Builder
# Internet Radio Project
# =====================================================

"""
Generate paged firmware station catalogues.

Responsibilities:
- Match stations to firmware filters
- Generate paged catalogue JSON files
- Build catalogue index.json
- Generate BBC and commercial filter catalogues
"""

import json
from pathlib import Path
from typing import Any

from bbc_builder import build_bbc_station_library
from common import (
    BUILD_TIMESTAMP,
    CATALOGUE_INDEX_FILE,
    CATALOGUE_PAGE_SIZE,
    CATALOGUE_VERSION,
    CATALOGUES_DIR,
    COUNTRY_CODE,
    VERSION,
)

FILTER_DEFINITIONS: dict[str, tuple[str, ...]] = {
    "absolute": ("absolute",),
    "virgin": ("virgin",),
    "capital": ("capital",),
    "heart": ("heart",),
    "kiss": ("kiss", "kisstory"),
    "rock": ("rock",),
    "talk": (
        "talk",
        "speech",
        "news",
        "current affairs",
        "politics",
        "lbc",
        "times radio",
    ),
}


def station_matches_filter(
    station: dict[str, Any],
    filter_name: str,
) -> bool:
    """Return whether a production station belongs in a UI filter."""

    if filter_name == "all":
        return True

    keywords = FILTER_DEFINITIONS[filter_name]

    searchable = " ".join(
        [
            str(station.get("name") or ""),
            str(station.get("tags") or ""),
        ]
    ).casefold()

    return any(keyword in searchable for keyword in keywords)


def write_catalogue_page(
    filter_name: str,
    page_number: int,
    page_count: int,
    station_count: int,
    stations: list[dict[str, Any]],
) -> Path:
    """Write one compact ESP32-ready catalogue page."""

    page_file = CATALOGUES_DIR / f"{filter_name}_{page_number - 1:03d}.json"

    with page_file.open("w", encoding="utf-8") as file:
        json.dump(
            {
                "filter": filter_name,
                "page": page_number,
                "page_count": page_count,
                "station_count": station_count,
                "stations": stations,
            },
            file,
            ensure_ascii=False,
            separators=(",", ":"),
        )

    return page_file


def write_filter_catalogues(
    library: list[dict[str, Any]],
    bbc_stations: list[dict[str, Any]],
) -> dict[str, dict[str, Any]]:
    """Write paged catalogue files matching the firmware UI filters."""

    CATALOGUES_DIR.mkdir(parents=True, exist_ok=True)

    for item in CATALOGUES_DIR.iterdir():
        if item.is_file():
            item.unlink()
        elif item.is_dir():
            import shutil

            shutil.rmtree(item)

    write_catalogue_page(
        filter_name="bbc",
        page_number=1,
        page_count=1,
        station_count=len(bbc_stations),
        stations=bbc_stations,
    )

    index_filters: dict[str, dict[str, Any]] = {
        "bbc": {
            "label": "BBC",
            "source": "littlefs",
            "station_count": len(bbc_stations),
            "page_count": 1,
            "page_size": CATALOGUE_PAGE_SIZE,
            "path": "/catalogues/bbc_%03d.json",
        }
    }

    filter_names = [
        "absolute",
        "virgin",
        "capital",
        "heart",
        "kiss",
        "rock",
        "talk",
        "all",
    ]

    for filter_name in filter_names:
        matching = [
            station
            for station in library
            if station_matches_filter(station, filter_name)
        ]

        station_count = len(matching)
        page_size = 5 if filter_name == "all" else CATALOGUE_PAGE_SIZE
        page_count = (station_count + page_size - 1) // page_size

        if page_count == 0:
            page_count = 1

        for page_index in range(page_count):
            start = page_index * page_size
            page_stations = matching[start : start + page_size]

            write_catalogue_page(
                filter_name=filter_name,
                page_number=page_index + 1,
                page_count=page_count,
                station_count=station_count,
                stations=page_stations,
            )

        index_filters[filter_name] = {
            "label": filter_name.title(),
            "source": "littlefs",
            "station_count": station_count,
            "page_count": page_count,
            "page_size": page_size,
            "path": f"/catalogues/{filter_name}_%03d.json",
        }

    with CATALOGUE_INDEX_FILE.open("w", encoding="utf-8") as file:
        json.dump(
            {
                "catalogue_version": CATALOGUE_VERSION,
                "build_timestamp": BUILD_TIMESTAMP,
                "tool_version": VERSION,
                "country": COUNTRY_CODE,
                "page_size": CATALOGUE_PAGE_SIZE,
                "filters": index_filters,
            },
            file,
            ensure_ascii=False,
            indent=2,
        )

    return index_filters
