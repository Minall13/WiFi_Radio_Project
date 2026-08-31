# Prerequisites

This document describes the software and development tools required to build, deploy, inspect and modify the Wi-Fi Radio Project.

Not every tool listed here is required for every task. The minimum requirements depend on whether you are:

- building and uploading the firmware;
- rebuilding the station catalogue and artwork;
- regenerating display fonts;
- modifying the PCB;
- modifying or printing the enclosure.

---

## 1. Arduino Development

### Arduino IDE

The firmware is developed and uploaded using **Arduino IDE**.

Install the current Arduino IDE from the official Arduino website.

### ESP32 Board Support

Install the **ESP32 Arduino board package** through Arduino IDE.

The project hardware uses an:

**ESP32-S3 DevKitC-1 N16R8**

with:

- 16 MB Flash;
- 8 MB PSRAM.

When compiling the project, ensure that the selected board and Flash/PSRAM settings are appropriate for the N16R8 module.

The project also uses the custom root-level `partitions.csv`, which provides approximately:

- 3 MB application space;
- 12.9 MB LittleFS space;
- no OTA application partition.

Further build and upload instructions are provided in `docs/Build_and_Deploy.md`.

---

## 2. Arduino Libraries

The finished firmware uses the following libraries:

- ArduinoJson
- ESP32-audioI2S (`Audio.h`)
- HTTPClient
- LittleFS
- PNGdec
- TFT_eSPI
- WiFi
- WiFiClientSecure
- WiFiManager
- XPT2046_Touchscreen

The following are provided as part of the ESP32 Arduino environment rather than installed as separate third-party libraries:

- SPI
- WiFi
- WiFiClientSecure
- HTTPClient
- LittleFS
- `esp_heap_caps.h`

Where a library version is important to project behaviour, use the version documented with the project.

The audio implementation was developed and validated using:

**ESP32-audioI2S 3.4.6**

---

## 3. Python

The Station Library Builder is written in Python.

Install a current **Python 3** release.

The Station Library Builder requires the Pillow Python package. It must be installed in the project's virtual environment before any Station Library Builder command can be run.

The recommended project setup is to use a Python virtual environment in the repository root.

Create the environment with:

`python3 -m venv .venv`

Activate it on macOS or Linux with:

`source .venv/bin/activate`

The Station Library Builder uses Python standard-library modules together with one external package:

**Pillow**

Install Pillow with:

`python -m pip install Pillow`

The Station Library Builder can then be run using:

`./.venv/bin/python tools/Station_Library_Builder/station_catalogue_manager.py build`

Detailed station catalogue and artwork information is provided in `docs/Station_Artwork.md`.

---

## 4. Processing IDE

The display uses TFT_eSPI Smooth Fonts stored as `.vlw` files.

To regenerate or modify these fonts, install:

**Processing IDE**

The font-generation workflow uses the TFT_eSPI Smooth Font generator, normally based on `Create_font.pde`.

The project uses the Google Roboto SemiCondensed font family.

The current generated font assets are stored in `data/fonts/`.

Detailed font-generation information is provided in `docs/FontGeneration.md`.

Processing is not required merely to compile or upload the existing firmware if the supplied `.vlw` files are retained.

---

## 5. Source-Code Editor

A separate source-code editor is optional.

The project can be edited entirely using Arduino IDE, but a general-purpose editor may be more convenient for:

- Python;
- JSON;
- Markdown documentation;
- repository-wide searches;
- project maintenance.

The project was developed using tools including **Visual Studio Code** and **BBEdit**, but neither is a mandatory dependency.

---

## 6. Git

Git is recommended when cloning or maintaining the project repository.

Typical uses include:

- downloading the source;
- tracking local modifications;
- updating from GitHub;
- maintaining personal branches or forks.

Git is not required simply to copy the source files and build the project manually.

---

## 7. KiCad

The custom carrier PCB was designed using:

**KiCad 10**

Install KiCad if you want to:

- inspect the schematic;
- inspect or modify the PCB;
- regenerate manufacturing files;
- create a revised board design.

The authoritative hardware design files are stored in `Hardware/`.

KiCad is not required to build firmware for an already assembled radio.

---

## 8. 3D Printing and CAD

The enclosure consists of three printable components:

- Base;
- Main Case;
- Face Frame.

Generic STL files are provided for use with compatible slicers and printers.

The supported Bambu-oriented print package is published separately through MakerWorld.

For Bambu printers, **Bambu Studio** or the Bambu Handy / MakerWorld workflow can be used.

Other slicers may be used with the supplied STL files.

A CAD application is only required if the enclosure geometry is to be modified.

---

## 9. USB Connection

Firmware can be uploaded directly to the ESP32-S3 through USB.

The finished radio provides rear USB-C access so firmware can be updated without disassembling the enclosure.

Before uploading firmware or LittleFS data:

- close Arduino Serial Monitor;
- close any other application using the ESP32 USB/serial port;
- confirm the correct USB device is selected.

---

## 10. Minimum Setup by Task

### Firmware only

Required:

- Arduino IDE;
- ESP32 Arduino board support;
- required Arduino libraries;
- USB connection to the ESP32-S3.

### Rebuild station catalogue and artwork

Additionally required:

- Python 3;
- Python virtual environment;
- Pillow.

### Regenerate fonts

Additionally required:

- Processing IDE;
- TFT_eSPI Smooth Font generator;
- required font family.

### Modify PCB

Additionally required:

- KiCad 10 or a compatible later version.

### Print enclosure

Required:

- suitable slicer or MakerWorld/Bambu workflow;
- compatible 3D printer.

---

## 11. Next Step

Once the required tools are installed, continue with `docs/Build_and_Deploy.md`.

For station catalogue and artwork preparation, see `docs/Station_Artwork.md`.