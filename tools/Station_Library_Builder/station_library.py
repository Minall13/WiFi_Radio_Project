# =====================================================
# Station Library
# Internet Radio Project
# =====================================================

"""
Build the production UK station library.

Responsibilities:
- Load the curated station manifest
- Select the best Radio Browser stream
- Validate supported stations
- Build the production station library
- Write station_library.json
"""

import json
from typing import Any

from bbc_builder import BBC_NATIONAL_STATIONS

from common import (
    BUILD_TIMESTAMP,
    CATALOGUE_VERSION,
    STATION_LIBRARY_FILE,
    VERSION,
)


def write_station_library(
    library: list[dict[str, Any]],
) -> None:
    """Write the production station library."""

    with STATION_LIBRARY_FILE.open("w", encoding="utf-8") as file:
        json.dump(
            {
                "catalogue_version": CATALOGUE_VERSION,
                "build_timestamp": BUILD_TIMESTAMP,
                "tool_version": VERSION,
                "country": "GB",
                "source": "Radio Browser",
                "bbc_separate": True,
                "station_count": len(library) + len(BBC_NATIONAL_STATIONS),
                "stations": library,
            },
            file,
            ensure_ascii=False,
            indent=2,
        )
