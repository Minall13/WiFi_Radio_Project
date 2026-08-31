from datetime import datetime, timezone
from pathlib import Path

VERSION = "1.0.2"

BUILD_TIME = datetime.now(timezone.utc)
CATALOGUE_VERSION = BUILD_TIME.strftime("%Y-%m-%d")
BUILD_TIMESTAMP = BUILD_TIME.isoformat(timespec="seconds").replace("+00:00", "Z")

TOOL_DIR = Path(__file__).resolve().parent
OUTPUT_DIR = TOOL_DIR / "output"
STATION_LIBRARY_FILE = OUTPUT_DIR / "station_library.json"

CATALOGUES_DIR = OUTPUT_DIR / "catalogues"
CATALOGUE_INDEX_FILE = CATALOGUES_DIR / "index.json"
CATALOGUE_PAGE_SIZE = 80
COUNTRY_CODE = "GB"
BUILD_REPORT_FILE = OUTPUT_DIR / "station_library_report.txt"
