# =====================================================
# Sync Assets
# Internet Radio Project
# =====================================================

"""Copy generated assets into the firmware data folder."""

import shutil
from pathlib import Path

from common import (
    CATALOGUES_DIR,
    OUTPUT_DIR,
    STATION_LIBRARY_FILE,
    TOOL_DIR,
)


def sync_firmware_assets() -> tuple[Path, Path, Path]:
    """Replace the LittleFS station assets with the latest build output."""

    project_dir = TOOL_DIR.parent.parent
    data_dir = project_dir / "data"
    data_catalogues_dir = data_dir / "catalogues"
    output_logos_dir = OUTPUT_DIR / "logos"
    data_logos_dir = data_dir / "logos"
    data_library_file = data_dir / "station_library.json"

    data_dir.mkdir(parents=True, exist_ok=True)

    if data_catalogues_dir.exists():
        shutil.rmtree(data_catalogues_dir)

    shutil.copytree(
        CATALOGUES_DIR,
        data_catalogues_dir,
    )

    if data_logos_dir.exists():
        shutil.rmtree(data_logos_dir)

    if output_logos_dir.exists():
        shutil.copytree(
            output_logos_dir,
            data_logos_dir,
        )

    shutil.copy2(
        STATION_LIBRARY_FILE,
        data_library_file,
    )

    return data_library_file, data_catalogues_dir, data_logos_dir
