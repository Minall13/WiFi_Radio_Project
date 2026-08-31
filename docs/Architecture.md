# Wi-Fi Radio Project — Architecture

## Overview

The Wi-Fi Radio Project firmware is organised as a modular ESP32-S3 application.

Functionality is divided across Arduino `.ino` tabs, with each tab responsible for a specific area of the system. The source files contain their own detailed header documentation; this document therefore describes the overall architecture and the relationships between the major parts of the application.

The principal design goal is to keep the user interface, station catalogue, audio playback and metadata systems independent of one another.

---

## High-Level Architecture

The application can be viewed as four main functional areas:

```text
User Interface / Touch
        │
        ▼
Station Provider
        │
        ├──────────────► Station Artwork
        │
        ▼
Audio Engine
        │
        ├──────────────► ICY Metadata
        │
        └──────────────► BBC Programme Metadata
```

Supporting modules provide Wi-Fi management, themes, typography, icons, startup behaviour and system information.

This separation allows individual parts of the radio to be changed without requiring corresponding changes throughout the application.

---

## Station Catalogue

Normal radio operation uses locally stored station catalogues held in LittleFS.

The catalogues are generated offline by the Python **Station Library Builder** and contain the information required by the firmware to present and play the curated station library.

The Station Provider acts as the interface between the user interface and these catalogue files.

This means that normal station browsing does not depend upon live access to an external station-directory service.

Only the catalogue data required at a particular time is loaded, reducing RAM usage on the ESP32-S3.

---

## Playback Model

Station browsing and station playback are deliberately treated as separate operations.

The application maintains independent state for:

- the currently selected station;
- the currently playing station;
- the current playback state.

This distinction allows the user to leave the Player screen and browse the Station List while the current station continues playing.

Consequently:

```text
Player → Back → Station List
```

preserves playback, whereas:

```text
Player → Stop → Filter Selection
```

terminates playback and returns to the beginning of the normal station-selection flow.

This behaviour is intentional and forms part of the application's navigation model.

---

## Audio

Audio streaming is handled independently by the Audio Engine.

The engine is responsible for:

- stream connection and playback;
- I²S audio output;
- volume control;
- mute state;
- playback status;
- stream metadata handling.

Audio is output through two MAX98357A I²S amplifier modules driving two speakers in a dual-mono configuration.

Keeping audio processing separate from the user interface allows playback to continue while other screens are displayed.

---

## Metadata

The project uses two separate metadata mechanisms.

### ICY Metadata

Commercial internet-radio streams may provide ICY metadata as part of the audio stream.

Where available, this information is used to display the current artist and track on the Player screen.

### BBC Programme Metadata

BBC stations use a separate metadata mechanism to obtain current programme information.

This information is handled independently from the audio stream and presented through the same Player interface.

Separating the two mechanisms allows the user interface to present consistent programme information despite the different metadata sources.

---

## Local Assets

LittleFS provides the radio's local runtime asset storage.

It contains resources including:

- station catalogues;
- station artwork;
- fonts;
- icons.

Keeping these resources locally provides predictable UI performance and avoids unnecessary network dependencies during normal navigation.

The project's custom flash partition configuration and LittleFS deployment process are documented separately in `Build_and_Deploy.md`.

---

## Design Principles

The firmware architecture follows a small number of deliberate principles:

- each module should have a clearly defined responsibility;
- UI rendering should remain separate from audio playback;
- navigation should not unnecessarily interrupt playback;
- station catalogue access should be abstracted from the UI;
- external metadata services should not control core navigation;
- runtime assets should be stored locally where practical;
- common visual behaviour should be controlled through shared theme, typography and icon systems.

The result is a firmware structure in which the major subsystems can be maintained independently while presenting a single consistent interface to the user.