# Internet Radio Project — Build and Deploy

This document describes how to prepare the software, generate the station catalogue, deploy the LittleFS filesystem and upload the firmware to an Internet Radio Project unit.

It assumes that the hardware has already been assembled and that the repository has been downloaded or cloned to the development computer.

---

## 1. Requirements

### Hardware

The original project uses:

- ESP32-S3 DevKitC-1 N16R8;
- 4-inch 480 × 320 ST7789 TFT display;
- XPT2046 resistive touchscreen;
- 2 × MAX98357A I²S amplifier modules;
- 2 × 3 W / 4 Ω speakers;
- Internet Radio Project carrier PCB.

See [`../BOM.md`](../BOM.md) for the complete bill of materials.

### Development Software

The build environment requires:

- Arduino IDE;
- ESP32 Arduino board package;
- Python 3;
- Python packages required by the Station Library Builder;
- `esptool`;
- the Arduino libraries used by the project.

The repository contains the project-specific tools required to build the station catalogue and LittleFS filesystem.

---

## 2. Obtain the Project

Clone or download the repository and open a Terminal in the project root.

The following files and directories should be visible:

```text
WiFi_Radio_Project.ino
data/
docs/
Hardware/
tools/
```

Commands in this document assume that the current Terminal directory is the project root.

Confirm this with:

```bash
pwd
```

---

## 3. Python Environment

The Station Library Builder is written in Python.

The development project uses a virtual environment named:

```text
.venv
```

If an appropriate virtual environment has already been created, the builder can be invoked directly using its Python interpreter:

```bash
./.venv/bin/python tools/Station_Library_Builder/station_catalogue_manager.py
```

Python environment requirements may vary according to platform and local installation.

---

## 4. Station Library Builder

The radio does not query an Internet radio directory during normal operation.

Instead, its station catalogue is prepared offline using the **Station Library Builder**.

From the project root, run:

```bash
./.venv/bin/python tools/Station_Library_Builder/station_catalogue_manager.py build
```

The builder prepares the runtime station data required by the ESP32.

Its tasks include:

1. Loading the source station information.
2. Applying the curated station definitions.
3. Preparing BBC station records.
4. Validating station information.
5. Preparing station artwork where available.
6. Generating filter catalogues.
7. Writing the station library.
8. Preparing the generated files for LittleFS deployment.

Warnings relating to unavailable external artwork do not necessarily indicate that the catalogue build has failed.

A successful build should finish with:

```text
✓ Ready for LittleFS upload.
```

---

## 5. Generated Catalogue Files

Generated runtime files are written beneath:

```text
data/
```

Important files include:

```text
data/station_library.json
data/catalogues/index.json
```

The catalogue directory can be inspected with:

```bash
ls -lh data/catalogues
```

The station catalogue is subsequently stored in the ESP32's LittleFS filesystem.

---

## 6. Station Artwork

The Station Library Builder also prepares artwork used by the Player interface.

Generated artwork is stored in the project's LittleFS data structure and is deployed with the station catalogue.

Station names, trademarks and logos remain the property of their respective owners.

A public distribution of this project may omit third-party station artwork. In that case, artwork should be obtained or generated locally using the Station Library Builder where appropriate.

---

## 7. UI Icon Generation

The Settings screen uses a small set of RAW565 status icons stored in the LittleFS filesystem.

The runtime icons used by the firmware are located in:

    data/icons/

These files can be regenerated from the source SVG artwork using the Processing-based Icon Builder supplied with the project:

    tools/Icon_Builder/

The important source files are:

    tools/Icon_Builder/SVG_to_RAW565.pde
    tools/Icon_Builder/icons/

Open `SVG_to_RAW565.pde` in the Processing IDE and run the sketch to generate the RAW565 icon variants.

Generated files are written to:

    tools/Icon_Builder/output/

The `output/` directory is generated locally and is not required to be stored in the repository.

Copy the required generated icon files into:

    data/icons/

Only the icons actually required by the firmware need to be placed there.

The files in `data/icons/` are subsequently included in the LittleFS image and uploaded to the ESP32-S3 with the rest of the runtime data.

UI icons only need to be regenerated when the icon artwork, size, colour or other graphical properties are changed.

---

## 8. Build LittleFS

The project includes a convenience helper for building and uploading the LittleFS filesystem.

Before starting, close Arduino Serial Monitor and any other application using the ESP32-S3 serial/USB connection.

From the project root, run:

    ./deploy

The root `deploy` helper invokes:

    tools/deploy_littlefs.sh

which performs the LittleFS build and upload process.

The underlying scripts are:

    tools/build_littlefs.sh
    tools/upload_littlefs.sh

The build process creates:

    build/littlefs.bin

The upload process then writes the filesystem image to the ESP32-S3 using `esptool`.

Successful deployment should finish with:

    LittleFS upload complete.

---

## 9. Script Permissions

On macOS or another Unix-like system, the deployment scripts must be executable.

If the shell reports:

```text
permission denied
```

run:

```bash
chmod +x tools/deploy_littlefs.sh \
  tools/build_littlefs.sh \
  tools/upload_littlefs.sh
```

Then retry:

```bash
./tools/deploy_littlefs.sh
```

---

## 10. USB Connection

The finished Internet Radio Project provides external USB-C access to the ESP32-S3.

This connection supports:

- 5 V power;
- USB data;
- firmware programming.

The completed radio can therefore be programmed without dismantling the enclosure.

During development, ensure that no other application is holding the ESP32 USB/serial port open.

---

## 11. Upload the Firmware

Open:

```text
WiFi_Radio_Project.ino
```

in Arduino IDE.

Confirm that:

1. The ESP32-S3 board support package is installed.
2. The correct ESP32-S3 board is selected.
3. The appropriate Flash and PSRAM settings are selected for the N16R8 module.
4. The project uses the custom `partitions.csv` supplied in the repository root. This provides a 3 MB application partition and approximately 12.9 MB for LittleFS.
5. The radio's USB port is selected under **Tools → Port**.

Click **Upload**.

Wait for Arduino IDE to report successful completion.

LittleFS and firmware are separate uploads. Uploading firmware does not normally require rebuilding the station catalogue unless its data has changed.

---

## 12. First Startup

After both LittleFS and firmware have been deployed, reset or power-cycle the radio.

The startup sequence should initialise:

```text
LittleFS
   ↓
Station data
   ↓
Wi-Fi
   ↓
BBC programme cache
   ↓
Audio
   ↓
Touch interface
```

The display reports startup progress.

When initialisation has completed, the radio displays:

```text
Ready
```

followed by:

```text
Touch anywhere to continue
```

---

## 13. Wi-Fi Provisioning

The radio requires a 2.4 GHz Wi-Fi connection.

Saved Wi-Fi credentials are reused automatically.

On a newly programmed ESP32-S3, or when valid credentials are unavailable, use the radio's Wi-Fi provisioning mechanism to select the required network and provide its credentials.

After successful provisioning, the credentials are stored by the ESP32 and normal startup should reconnect automatically.

---

## 14. Functional Test

After deployment, perform a basic functional test.

Confirm that:

1. The startup sequence completes.
2. Wi-Fi connects successfully.
3. Touching the startup screen opens the Filter screen.
4. Station filters open correctly.
5. Station lists display correctly.
6. A BBC station can be selected.
7. BBC programme information appears where available.
8. Audio plays correctly.
9. A commercial station can be selected.
10. ICY metadata appears where supplied by the broadcaster.
11. Station artwork displays correctly.

On the Player screen test:

- **Back**
- **Stop**
- **Volume −**
- **MUTE**
- **Volume +**

Expected navigation behaviour:

```text
Back → Station List
       Audio continues

Stop → Filter
       Audio stops
```

---

## 15. Serial Diagnostics

For development and troubleshooting, Arduino Serial Monitor can be used to observe the radio during startup and playback.

Useful checks include:

- LittleFS mounts successfully;
- catalogue index loads successfully;
- station catalogue pages load correctly;
- Wi-Fi connects;
- BBC metadata caching completes;
- audio initialises;
- selected streams connect;
- no unexpected ESP32 errors are reported.

Close Serial Monitor before attempting another LittleFS upload if it prevents access to the USB port.

---

## 16. Rebuilding the Station Catalogue

Rebuild the catalogue whenever station definitions, stream URLs, filters or artwork sources are changed.

From the project root:

```bash
./.venv/bin/python tools/Station_Library_Builder/station_catalogue_manager.py build
```

Then redeploy LittleFS:

```bash
./tools/deploy_littlefs.sh
```

Restart the radio after deployment.

Firmware does not need to be recompiled solely because catalogue data has changed.

---

## 17. Combined Catalogue and LittleFS Deployment

On a development system using the project's `.venv`, the catalogue build and LittleFS deployment can be chained:

```bash
./.venv/bin/python tools/Station_Library_Builder/station_catalogue_manager.py build && \
./tools/deploy_littlefs.sh
```

The LittleFS deployment will only begin if the Station Library Builder exits successfully.

Firmware upload remains a separate Arduino IDE operation.

---

## 18. Troubleshooting

### LittleFS upload cannot access the ESP32

Close:

- Arduino Serial Monitor;
- other serial terminals;
- applications using the ESP32 USB port.

Then retry the deployment.

### Station does not play

Internet radio stream URLs can change without notice.

Check whether the broadcaster has changed:

- stream URL;
- streaming protocol;
- codec;
- geographic availability.

Update the curated station definition and rebuild the catalogue if necessary.

### Artwork cannot be obtained

Artwork sources can also change or disappear.

The Station Library Builder can use alternative or default artwork where a suitable station image is unavailable.

### Wi-Fi does not connect

If stored credentials are no longer valid, repeat Wi-Fi provisioning using the required network.

---

## 19. Deployment Summary

The complete software deployment process is:

```text
Prepare development environment
          ↓
Build station catalogue
          ↓
Build LittleFS image
          ↓
Upload LittleFS
          ↓
Compile firmware
          ↓
Upload firmware
          ↓
Configure Wi-Fi if required
          ↓
Functional test
```

Once deployment and Wi-Fi configuration are complete, the radio operates as a standalone appliance and does not require the development computer for normal use.