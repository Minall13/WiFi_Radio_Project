# =====================================================
# BBC Builder
# Internet Radio Project
# =====================================================

"""Generate the BBC station catalogue and metadata."""

import re
import urllib.error
import urllib.request
from typing import Any

from common import VERSION

APP_NAME = "InternetRadioProject-StationCatalogueManager"
REQUEST_TIMEOUT_SECONDS = 30

BBC_NATIONAL_STATIONS: list[dict[str, Any]] = [
    {
        "id": "bbc_radio_1",
        "name": "BBC Radio 1",
        "metadata_service": "bbc_radio_one",
        "url": (
            "http://as-hls-ww-live.akamaized.net/"
            "pool_01505109/live/ww/bbc_radio_one/"
            "bbc_radio_one.isml/"
            "bbc_radio_one-audio%3d96000.norewind.m3u8"
        ),
        "codec": "AAC",
        "bitrate": 96,
        "homepage": (
            "https://www.bbc.co.uk/sounds/play/"
            "live:bbc_radio_one"
        ),
    },
    {
        "id": "bbc_radio_2",
        "name": "BBC Radio 2",
        "metadata_service": "bbc_radio_two",
        "url": (
            "http://as-hls-ww-live.akamaized.net/"
            "pool_74208725/live/ww/bbc_radio_two/"
            "bbc_radio_two.isml/"
            "bbc_radio_two-audio%3d96000.norewind.m3u8"
        ),
        "codec": "AAC",
        "bitrate": 96,
        "homepage": (
            "https://www.bbc.co.uk/sounds/play/"
            "live:bbc_radio_two"
        ),
    },
    {
        "id": "bbc_radio_3",
        "name": "BBC Radio 3",
        "metadata_service": "bbc_radio_three",
        "url": (
            "http://as-hls-ww-live.akamaized.net/"
            "pool_23461179/live/ww/bbc_radio_three/"
            "bbc_radio_three.isml/"
            "bbc_radio_three-audio%3d96000.norewind.m3u8"
        ),
        "codec": "AAC",
        "bitrate": 96,
        "homepage": (
            "https://www.bbc.co.uk/sounds/play/"
            "live:bbc_radio_three"
        ),
    },
    {
        "id": "bbc_radio_4",
        "name": "BBC Radio 4",
        "metadata_service": "bbc_radio_fourfm",
        "url": (
            "http://as-hls-ww-live.akamaized.net/"
            "pool_55057080/live/ww/bbc_radio_fourfm/"
            "bbc_radio_fourfm.isml/"
            "bbc_radio_fourfm-audio%3d96000.norewind.m3u8"
        ),
        "codec": "AAC",
        "bitrate": 96,
        "homepage": (
            "https://www.bbc.co.uk/sounds/play/"
            "live:bbc_radio_fourfm"
        ),
    },
    {
        "id": "bbc_radio_5_live",
        "name": "BBC Radio 5 Live",
        "metadata_service": "bbc_radio_five_live",
        "url": (
            "http://as-hls-ww-live.akamaized.net/"
            "pool_89021708/live/ww/bbc_radio_five_live/"
            "bbc_radio_five_live.isml/"
            "bbc_radio_five_live-audio%3d96000.norewind.m3u8"
        ),
        "codec": "AAC",
        "bitrate": 96,
        "homepage": (
            "https://www.bbc.co.uk/sounds/play/"
            "live:bbc_radio_five_live"
        ),
    },
    {
        "id": "bbc_radio_6_music",
        "name": "BBC Radio 6 Music",
        "metadata_service": "bbc_6music",
        "url": (
            "http://as-hls-ww-live.akamaized.net/"
            "pool_81827798/live/ww/bbc_6music/"
            "bbc_6music.isml/"
            "bbc_6music-audio%3d96000.norewind.m3u8"
        ),
        "codec": "AAC",
        "bitrate": 96,
        "homepage": (
            "https://www.bbc.co.uk/sounds/play/"
            "live:bbc_6music"
        ),
    },
    {
        "id": "bbc_radio_1xtra",
        "name": "BBC Radio 1Xtra",
        "metadata_service": "bbc_1xtra",
        "url": (
            "http://as-hls-ww-live.akamaized.net/"
            "pool_92079267/live/ww/bbc_1xtra/"
            "bbc_1xtra.isml/"
            "bbc_1xtra-audio%3d96000.norewind.m3u8"
        ),
        "codec": "AAC",
        "bitrate": 96,
        "homepage": (
            "https://www.bbc.co.uk/sounds/play/"
            "live:bbc_1xtra"
        ),
    },
    {
        "id": "bbc_radio_4_extra",
        "name": "BBC Radio 4 Extra",
        "metadata_service": "bbc_radio_four_extra",
        "url": (
            "http://as-hls-ww-live.akamaized.net/"
            "pool_26173715/live/ww/bbc_radio_four_extra/"
            "bbc_radio_four_extra.isml/"
            "bbc_radio_four_extra-audio%3d96000."
            "norewind.m3u8"
        ),
        "codec": "AAC",
        "bitrate": 96,
        "homepage": (
            "https://www.bbc.co.uk/sounds/play/"
            "live:bbc_radio_four_extra"
        ),
    },
    {
        "id": "bbc_radio_5_sports_extra",
        "name": "BBC Radio 5 Sports Extra",
        "metadata_service": "bbc_radio_five_live_sports_extra",
        "url": (
            "http://as-hls-uk-live.akamaized.net/"
            "pool_47700285/live/uk/"
            "bbc_radio_five_live_sports_extra/"
            "bbc_radio_five_live_sports_extra.isml/"
            "bbc_radio_five_live_sports_extra-"
            "audio%3d96000.norewind.m3u8"
        ),
        "codec": "AAC",
        "bitrate": 96,
        "homepage": (
            "https://www.bbc.co.uk/sounds/play/"
            "live:bbc_radio_five_live_sports_extra"
        ),
    },
    {
        "id": "bbc_asian_network",
        "name": "BBC Asian Network",
        "metadata_service": "bbc_asian_network",
        "url": (
            "http://as-hls-ww-live.akamaized.net/"
            "pool_22108647/live/ww/bbc_asian_network/"
            "bbc_asian_network.isml/"
            "bbc_asian_network-audio%3d96000."
            "norewind.m3u8"
        ),
        "codec": "AAC",
        "bitrate": 96,
        "homepage": (
            "https://www.bbc.co.uk/sounds/play/"
            "live:bbc_asian_network"
        ),
    },
]


def fetch_open_graph_image(page_url: str) -> str:
    """Return the official Open Graph image from a BBC page."""

    request = urllib.request.Request(
        page_url,
        headers={
            "User-Agent": (f"{APP_NAME}/{VERSION} " "(station catalogue builder)"),
            "Accept": "text/html,application/xhtml+xml",
        },
    )

    try:
        with urllib.request.urlopen(
            request,
            timeout=REQUEST_TIMEOUT_SECONDS,
        ) as response:
            html = response.read().decode(
                "utf-8",
                errors="replace",
            )

    except (urllib.error.HTTPError, urllib.error.URLError) as error:
        print(f"Warning: artwork fetch failed for {page_url}: " f"{error}")
        return ""

    patterns = (
        r'<meta[^>]+property=["\']og:image["\'][^>]+' r'content=["\']([^"\']+)["\']',
        r'<meta[^>]+content=["\']([^"\']+)["\'][^>]+' r'property=["\']og:image["\']',
    )

    for pattern in patterns:
        match = re.search(
            pattern,
            html,
            flags=re.IGNORECASE,
        )

        if match:
            return match.group(1).strip()

    print(f"Warning: no og:image metadata found for {page_url}")
    return ""


def build_bbc_station_library() -> list[dict[str, Any]]:
    """Build BBC records and fetch official BBC artwork."""

    stations: list[dict[str, Any]] = []

    for definition in BBC_NATIONAL_STATIONS:
        homepage = str(definition.get("homepage") or "").strip()

        favicon = fetch_open_graph_image(homepage)

        print(
            f"BBC metadata: "
            f"{definition['name']:<26} "
            f"{'artwork found' if favicon else 'no artwork'}"
        )

        stations.append(
            {
                "id": definition["id"],
                "name": definition["name"],
                "filters": ["bbc"],
                "url": definition["url"],
                "codec": definition["codec"],
                "bitrate": definition["bitrate"],
                "homepage": homepage,
                "metadata_service": definition["metadata_service"],
                "favicon": favicon,
                "state": "",
                "tags": "bbc",
                "stationuuid": "",
            }
        )

    return stations
