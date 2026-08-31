"""
Station logo builder
Internet Radio Project

Downloads station artwork, resizes it and creates
TFT-ready PNG logo assets for LittleFS.
"""

from __future__ import annotations

import io
import urllib.error
import urllib.request
from pathlib import Path
from typing import Any
from dataclasses import dataclass

from PIL import Image, ImageDraw

Station = dict[str, Any]

@dataclass(frozen=True)
class LogoBuildResult:
    path: str
    status: str
    detail: str = ""

LOGO_SIZE = 190
CORNER_RADIUS = 14
MIN_SOURCE_DIMENSION = 80
REQUEST_TIMEOUT_SECONDS = 30
ASSETS_DIR = Path(__file__).resolve().parent / "assets"
CURATED_LOGOS_DIR = Path(__file__).resolve().parent / "curated_logos"
DEFAULT_LOGO = ASSETS_DIR / "house_logo.png"
DEFAULT_LOGO_FILENAME = "default_station.png"


def build_default_logo(output_dir: Path) -> str:
    """Build the project fallback logo and return its LittleFS path."""

    if not DEFAULT_LOGO.exists():
        raise RuntimeError(f"Default logo source not found: {DEFAULT_LOGO}")

    image = Image.open(DEFAULT_LOGO)
    image.load()

    prepared = prepare_logo(image)

    output_path = output_dir / DEFAULT_LOGO_FILENAME
    write_png(prepared, output_path)

    return f"/logos/{DEFAULT_LOGO_FILENAME}"


def download_image(image_url: str) -> Image.Image:
    """Download a station image and return it as a Pillow image."""

    request = urllib.request.Request(
        image_url,
        headers={
            "User-Agent": "InternetRadioProject-StationCatalogueManager",
            "Accept": "image/*",
        },
    )

    try:
        with urllib.request.urlopen(
            request,
            timeout=REQUEST_TIMEOUT_SECONDS,
        ) as response:
            image_data = response.read()

    except (urllib.error.HTTPError, urllib.error.URLError) as error:
        raise RuntimeError(f"Unable to download logo: {error}") from error

    try:
        image = Image.open(io.BytesIO(image_data))
        image.load()

    except Exception as error:
        raise RuntimeError("Downloaded logo is not a supported image") from error

    return image


def prepare_logo(image: Image.Image) -> Image.Image:
    """Convert, resize and round an image into a square station logo."""

    image = image.convert("RGBA")

    image.thumbnail(
        (LOGO_SIZE, LOGO_SIZE),
        Image.Resampling.LANCZOS,
    )

    canvas = Image.new(
        "RGBA",
        (LOGO_SIZE, LOGO_SIZE),
        (0, 0, 0, 255),
    )

    x = (LOGO_SIZE - image.width) // 2
    y = (LOGO_SIZE - image.height) // 2

    canvas.alpha_composite(image, (x, y))

    mask = Image.new(
        "L",
        (LOGO_SIZE, LOGO_SIZE),
        0,
    )

    ImageDraw.Draw(mask).rounded_rectangle(
        (0, 0, LOGO_SIZE - 1, LOGO_SIZE - 1),
        radius=CORNER_RADIUS,
        fill=255,
    )

    canvas.putalpha(mask)

    return canvas


def write_png(image: Image.Image, output_path: Path) -> None:
    """Write an RGBA PNG image for LittleFS."""

    output_path.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    image.save(
        output_path,
        format="PNG",
        optimize=False,
    )


def build_station_logo(
    station: Station,
    output_dir: Path,
) -> LogoBuildResult:
    """Create one local PNG logo and return its build result."""

    station_id = str(station.get("id") or "").strip()
    favicon = str(station.get("favicon") or "").strip()

    if not station_id:
        raise RuntimeError("Station logo record is missing an id")

    curated_path = CURATED_LOGOS_DIR / f"{station_id}.png"

    if curated_path.exists():
        try:
            image = Image.open(curated_path)
            image.load()
        except Exception as error:
            raise RuntimeError(
                f"Curated logo is not a supported image: {curated_path.name}"
            ) from error

        prepared = prepare_logo(image)

        output_path = output_dir / curated_path.name

        write_png(
            prepared,
            output_path,
        )

        return LogoBuildResult(
            path=f"/logos/{curated_path.name}",
            status="curated",
        )

    if favicon.casefold() in {"", "null", "none"}:
        return LogoBuildResult(
            path="/logos/default_station.png",
            status="default",
            detail="No favicon URL",
        )

    image = download_image(favicon)

    if min(image.size) < MIN_SOURCE_DIMENSION:
        return LogoBuildResult(
            path="/logos/default_station.png",
            status="default",
            detail=f"Source image too small ({image.width}×{image.height})",
        )

    prepared = prepare_logo(image)

    filename = f"{station_id}.png"
    output_path = output_dir / filename

    write_png(
        prepared,
        output_path,
    )

    return LogoBuildResult(
        path=f"/logos/{filename}",
        status="downloaded",
    )