# Radio Browser

## Purpose

Radio Browser provides the source data used to build the Internet Radio station catalogue.

It is **not** used during normal operation of the radio.

Instead, the Python **Station Library Builder** processes, validates and prepares station data before generating the production catalogue stored in LittleFS.
---

## Current Workflow

```text
Radio Browser API
        │
        ▼
Station Library Builder (Python)
        │
        ▼
station_library.json
        │
        ▼
Paged catalogue files
(catalogues/*.json)
        │
        ▼
LittleFS Image
        │
        ▼
ESP32 Internet Radio
```

---

## Responsibilities

### Radio Browser

- Provides UK station metadata
- Provides stream URLs
- Provides station tags and codec information
- Provides station logos (where available)

### Wi-Fi Radio Project

- Selects supported UK stations
- Removes duplicate entries
- Builds production catalogue files
- Stores the catalogue in LittleFS
- Loads catalogue pages during runtime

---

## Runtime Behaviour

During normal operation the ESP32 **never queries Radio Browser**.

Station browsing is performed entirely from the locally stored catalogue.

This provides:

- Faster station browsing
- Reduced network traffic
- Consistent station ordering
- Offline browsing capability (network required only for audio streaming)
