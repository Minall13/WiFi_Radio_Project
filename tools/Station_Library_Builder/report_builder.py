# =====================================================
# Report Builder
# Internet Radio Project
# =====================================================

"""Generate station reports."""

from collections import Counter
from typing import Any

from common import (
    BUILD_REPORT_FILE,
    BUILD_TIMESTAMP,
    CATALOGUE_VERSION,
    VERSION,
)


def write_build_report(
    source_count: int,
    library: list[dict[str, Any]],
    counts: Counter[str],
) -> None:
    """Write a concise production-library build report."""

    lines = [
        "Station Catalogue Manager",
        "Production Station Library",
        "==========================",
        "",
        f"Manager version:          {VERSION}",
        f"Catalogue version:        {CATALOGUE_VERSION}",
        f"Build timestamp:          {BUILD_TIMESTAMP}",
        "",
        f"Source GB records:        {source_count}",
        f"Eligible supported:       {counts['eligible supported records']}",
        f"Unique station names:     {counts['unique normalised names']}",
        f"Final unique stations:    {counts['final unique stations']}",
        "",
        "Excluded records",
        "----------------",
    ]

    excluded = [
        (reason, count)
        for reason, count in counts.items()
        if reason
        not in {
            "eligible supported records",
            "unique normalised names",
            "final unique stations",
        }
    ]

    for reason, count in sorted(
        excluded,
        key=lambda item: (-item[1], item[0]),
    ):
        lines.append(f"{reason:<42}{count:>6}")

    lines.extend(
        [
            "",
            "Production policy",
            "-----------------",
            "BBC stations remain separate.",
            "Only direct, currently healthy MP3, AAC and AAC+ records are included.",
            "HLS, video, OGG and unknown codecs are excluded.",
            "Exact duplicate names and duplicate resolved URLs are collapsed.",
            "",
        ]
    )

    BUILD_REPORT_FILE.write_text(
        "\n".join(lines),
        encoding="utf-8",
    )
