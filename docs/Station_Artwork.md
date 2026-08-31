# Station Artwork

This document describes how station artwork is prepared for the Wi-Fi Radio Project and how to add or replace artwork when rebuilding the station library.

Station artwork is generated as part of the **Station Library Builder** workflow.

Third-party station logos and trademarks are **not distributed with the public repository**. Users may provide their own artwork where they have permission to use it, or allow the builder to obtain suitable station artwork where available.

If no suitable artwork can be obtained, the radio automatically uses the project's generic station image.

---

## 1. Overview

The Player interface displays a 190 × 190 pixel image for the selected station.

Artwork used by the firmware is stored in:

`data/logos/`

The Station Library Builder prepares this directory when the production station library is built.

For each station, the builder attempts to obtain artwork in the following order:

1. a locally supplied curated image;
2. the station favicon supplied by the station catalogue;
3. the project's generic default station image.

This means that a missing station logo does not prevent the station library from being built or the radio from operating.

---

## 2. Station IDs and Artwork Filenames

The authoritative curated station definitions are stored in:

`tools/Station_Library_Builder/curated_stations.json`

Each station has a unique `id`.

For example:

`"id": "planet_rock"`

The corresponding curated artwork filename is:

`planet_rock.png`

There is therefore no separate artwork filename manifest to maintain.

The station ID in `curated_stations.json` determines the expected filename automatically.

---

## 3. Curated Artwork

User-supplied station artwork is placed in:

`tools/Station_Library_Builder/curated_logos/`

The filename must correspond exactly to the station ID.

For example, if the station definition contains:

`"id": "planet_rock"`

the curated artwork should be:

`tools/Station_Library_Builder/curated_logos/planet_rock.png`

Curated artwork takes priority over automatically downloaded artwork.

The public repository contains a `.gitkeep` file in this directory so that the otherwise-empty directory is retained by Git.

Third-party station logos should only be added where the user has the appropriate rights or permission to use them.

---

## 4. Automatically Downloaded Artwork

If no curated image exists for a station, the builder checks the station's `favicon` value.

Where a favicon URL is available, the builder attempts to download the image.

Downloaded artwork must have a minimum source dimension of 80 pixels.

If the downloaded image is suitable, it is processed and written to:

`data/logos/<station_id>.png`

For example:

`data/logos/planet_rock.png`

If the favicon is unavailable, too small, cannot be downloaded, or cannot be interpreted as a supported image, the build process falls back to the generic station artwork.

---

## 5. Default Artwork

The source image for the project's generic fallback artwork is:

`tools/Station_Library_Builder/assets/house_logo.png`

During a build, the Station Library Builder processes this image and creates:

`data/logos/default_station.png`

Stations without suitable individual artwork use:

`/logos/default_station.png`

This ensures that every station has valid artwork even when no station-specific image is available.

---

## 6. Image Processing

Station artwork is normalised automatically by the builder.

The current artwork-processing parameters are:

- output size: 190 × 190 pixels;
- corner radius: 14 pixels;
- minimum downloaded source dimension: 80 pixels.

Images are converted to RGBA format and resized while preserving their aspect ratio.

The resized image is centred on a 190 × 190 pixel canvas.

A rounded-corner mask is then applied before the final PNG is written.

This processing gives the Player interface consistent artwork dimensions even when source images have different sizes or aspect ratios.

---

## 7. Source Artwork and Generated Artwork

It is useful to distinguish between the two artwork locations.

### Source artwork

`tools/Station_Library_Builder/curated_logos/`

contains optional user-supplied source artwork.

These files are inputs to the build process.

### Generated artwork

`data/logos/`

contains artwork prepared for use by the ESP32 firmware.

These files are build outputs.

The builder may create files in `data/logos/` from:

- curated artwork;
- downloaded station favicons;
- the generic house artwork.

The firmware uses the generated files in `data/logos/`, not the source files in `curated_logos/`.

---

## 8. Adding Artwork for an Existing Station

To provide your own artwork for a station:

1. Open `tools/Station_Library_Builder/curated_stations.json`.
2. Find the required station.
3. Note its `id`.
4. Prepare a PNG image that you are entitled to use.
5. Name the file `<station_id>.png`.
6. Place it in `tools/Station_Library_Builder/curated_logos/`.
7. Rebuild the station library.

For example, for:

`"id": "planet_rock"`

provide:

`tools/Station_Library_Builder/curated_logos/planet_rock.png`

There is no need to resize the image manually to exactly 190 × 190 pixels. The builder performs the final sizing and formatting.

---

## 9. Building the Station Library

The Station Library Builder requires Python 3 and Pillow.

The recommended setup is described in:

`docs/Prerequisites.md`

From the repository root, the complete production build is run with:

`./.venv/bin/python tools/Station_Library_Builder/station_catalogue_manager.py build`

The `build` command does not require additional command-line options.

As part of the build, the tool prepares the production station data and artwork required by the firmware.

The process includes:

1. loading the curated station definitions;
2. obtaining the required station information;
3. preparing the generic default artwork;
4. selecting curated or downloaded artwork for each station;
5. processing station artwork;
6. writing filter catalogues;
7. writing the production station library;
8. generating the build report;
9. synchronising the required firmware assets.

The resulting runtime station library is written beneath:

`data/`

Detailed build and deployment instructions are provided in:

`docs/Build_and_Deploy.md`

---

## 10. Build Messages

During the build, the Station Library Builder reports how artwork was obtained.

A station image may be reported as:

- curated;
- downloaded;
- default.

A warning about an unavailable or unsuitable station image does not necessarily mean that the build has failed.

Where appropriate, the builder substitutes the default station artwork and continues.

The build report should be reviewed after rebuilding the catalogue, particularly after adding or changing stations.

---

## 11. Adding a New Station

Artwork is only one part of adding a station.

The station must first have a valid entry in:

`tools/Station_Library_Builder/curated_stations.json`

The station's `id` becomes the basis of its local artwork filename.

Once the station definition has been added, you may either:

- provide a matching PNG in `curated_logos/`;
- allow the builder to use a suitable station favicon;
- allow the station to use the default artwork.

After modifying the curated station definitions, rebuild the production station library before deploying LittleFS to the radio.

---

## 12. BBC Stations

BBC stations are handled separately by the Station Library Builder rather than being treated as ordinary Radio Browser catalogue entries.

The production build includes the project's supported BBC national services through the dedicated BBC builder.

Artwork for BBC services follows the same general principle as other third-party station branding: third-party logos are not distributed as part of the public repository.

Where station-specific artwork is unavailable, the generic default artwork provides a safe runtime fallback.

---

## 13. Artwork Licensing

Radio station names, logos and other branding may be protected by copyright, trademark or other rights belonging to their respective owners.

The Wi-Fi Radio Project does not grant permission to redistribute third-party station artwork.

The public repository therefore provides:

- the artwork-generation tools;
- the curated station definitions;
- the project-owned generic fallback artwork;
- the generated generic `default_station.png`;

but does not intentionally distribute third-party station logos.

Users who add their own curated artwork are responsible for ensuring that they have the appropriate rights or permission to use it.

---

## 14. Summary

The station artwork workflow is deliberately designed so that third-party artwork is optional.

For each station, the builder uses:

**curated PNG → downloaded favicon → default artwork**

The station ID in `curated_stations.json` determines the curated artwork filename.

Source artwork belongs in:

`tools/Station_Library_Builder/curated_logos/`

Generated firmware artwork belongs in:

`data/logos/`

The complete production build is performed with:

`./.venv/bin/python tools/Station_Library_Builder/station_catalogue_manager.py build`

Once the build is complete, follow `docs/Build_and_Deploy.md` to deploy the generated LittleFS content and firmware.