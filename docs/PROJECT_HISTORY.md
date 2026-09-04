# Project History

## v0.1-display-touch-working

**Date:** 19-Jun-2026

**Git Tag:** `v0.1-display-touch-working`

### Objectives Achieved

* ESP32-S3 N16R8 board verified
* 4" TFT display operational
* XPT2046 touchscreen operational
* GitHub repository established
* SSH authentication configured
* Project documentation structure created
* Prototype test sketches archived

### Key Discoveries

#### Display

Display marketed as ILI9488.

Working TFT_eSPI configuration:

```cpp
#define USE_HSPI_PORT
#define ILI9481_DRIVER
```

Display would not operate correctly using:

```cpp
#define ILI9488_DRIVER
```

#### Touchscreen

Working wiring:

```text
T_DO  -> GPIO13
T_DIN -> GPIO11
T_CLK -> GPIO12
T_CS  -> GPIO14
T_IRQ -> Not connected
```

Critical discovery:

```text
TFT SDO(MISO) disconnected
```

If TFT SDO(MISO) is connected, the touchscreen reports continuously:

```text
Touched = YES | X = 0 | Y = 0 | Z = 4095
```

### Calibration Values

```cpp
#define TOUCH_MIN_X 300
#define TOUCH_MAX_X 3820
#define TOUCH_MIN_Y 420
#define TOUCH_MAX_Y 3770
```

### Test Sketches Archived

* 01_TFT_Colour_Test
* 02_Touch_Raw_Test
* 03_Touch_Calibration_Test

### Status

PASS — Display and touchscreen operational and documented.

---

## v0.2-touch-ui-input-working

**Date:** 20-Jun-2026

**Git Tag:** `v0.2-touch-ui-input-working`

### Objectives Achieved

* Touch coordinate mapping verified
* Stations button detection working
* Settings button detection working
* Repeated touch presses working
* Serial feedback operational

### Results

Verified:

* Touch detection
* Coordinate mapping
* Button hit detection
* Repeated touch presses
* Serial feedback

### Test Sketch Archived

* 04_Touch_UI_Input_Test

### Status

PASS — Touch UI input framework operational.

---

## v0.2.1-touch-ui-display-feedback

**Date:** 20-Jun-2026

**Git Tag:** `v0.2.1-touch-ui-display-feedback`

### Objective

Resolve TFT redraw after touch events whilst maintaining reliable touch input.

### Status

PASS — TFT redraw and touch input operate reliably together.

---

## v0.3.0-wifi-connected-basic

**Date:** 21-Jun-2026

### Status

PASS — Basic Wi‑Fi connectivity operational.

---

## v0.3.1-wifi-manager-portal

**Date:** 21-Jun-2026

### Status

PASS — Complete Wi‑Fi configuration workflow operational.

---

## v0.4-radio-browser-browser-ui

**Date:** 23–24 Jun 2026

### Objectives Achieved

* Radio Browser API integrated
* API server failover implemented
* Live station retrieval verified
* TFT station browser implemented
* Touchscreen page navigation implemented
* Filter-based station browsing implemented
* Station name and stream URL storage implemented
* Screen architecture framework implemented
* Footer station selector framework implemented
* Station visual state framework implemented

### Results

Verified:

* Successful retrieval of UK radio station data from Radio Browser
* API failover between multiple Radio Browser servers
* Storage and display of station names and stream URLs
* Storage of up to 40 stations
* TFT station browser display
* Touchscreen navigation using graphical ▲ and ▼ buttons
* 5 stations per page layout adopted
* Multi-page station browsing with wrap-around navigation
* Direct filter selection screen implemented

### Screen Architecture

Display divided into fixed zones:

```text
Header   50px
Content 190px
Footer   80px
```

This architecture will be retained for all future screens.

### Footer Navigation Framework

Introduced persistent footer layout:

```text
Home  [1] [2] [3] [4] [5]  Settings
```

Design objectives:

* Large touch targets
* No precision text tapping
* Consistent navigation across all screens
* Appliance-style operation

### Station Selection Philosophy

Station lists are numbered locally per page:

Example:

```text
1. Capital Chill
2. Capital Dance
3. Capital FM Birmingham
4. Capital FM AAC
5. Capital XTRA
```

Footer buttons remain:

```text
[1] [2] [3] [4] [5]
```

Page number provides position within the overall result set.

### Button Visual States

Implemented framework for:

#### Available

* Blue outline
* Blue numeral

Current colour:

```cpp
tft.color565(0,90,220)
```

#### Selected

* Green outline
* Green numeral
* Green glow effect

Current implementation uses a simple circular glow.

#### Disabled

* Grey outline
* Grey numeral

Used for unavailable station slots on short final pages.

### Radio Browser Reliability

Observed behaviour:

* Radio Browser servers occasionally return:

  * HTTP -11
  * HTTP -1
  * HTTP 502

API failover successfully recovers by attempting alternative servers.

Future UI versions should present loading and retry progress visually rather than relying on serial output.

### User Interface Direction

Reference UI concept documented in:

```text
docs/UI_Concept_v1.md
images/Internet_Radio_UI_Concept_v1.png
```

Key design goals:

* Clean appliance-style interface
* Persistent navigation
* Consistent visual language
* Large touch targets
* Future anti-aliased typography
* Future Now Playing screen

### Status

PASS — Radio Browser integration validated.

PASS — Station browser architecture established.

PASS — UI framework established.

Audio playback integration can now proceed on a stable foundation.


---

## v0.4.10-footer-selection-logic

**Date:** 26-Jun-2026

**Git Branch:** `v0.4.10-footer-selection-logic`

### Objectives Achieved

* Footer buttons 1–5 now select visible station entries
* Selected station is tracked using `selectedStationIndex`
* Selected footer button remains green
* Previous selected button returns to blue
* Unused station buttons are disabled on short final pages
* Legacy row-touch station selection removed
* Player screen introduced using `SCREEN_PLAYER`
* Station metadata model expanded
* Radio Browser metadata captured and printed for analysis
* Project refactored into multiple Arduino `.ino` tabs
* Project documentation reorganised into `docs/`
* Hardware notes moved into `Hardware/`
* Architecture documentation created

### Station Metadata Added

The `Station` structure now stores:

```cpp
name
streamUrl
resolvedUrl
homepage
favicon
tags
codec
bitrate
isHLS
```

### Screen Architecture

The project now uses a modular screen structure:

```text
Header  = status/context
Content = current task
Footer  = navigation
```

### Arduino Tab Structure

```text
Internet_Radio_Project.ino
01_Debug.ino
02_Display_UI.ino
03_Touch_Input.ino
04_WiFi_Manager.ino
05_RadioBrowser.ino
06_BBC_Stations.ino
07_Player.ino
08_StationProvider.ino
09_UI_Theme_Design.ino
```

### Status

PASS — Footer station selection working.

PASS — Player screen introduced.

PASS — Station metadata storage working.

PASS — Project refactored into maintainable Arduino tabs.

---

## v0.5.0-ui-design-system

**Date:** 27-Jun-2026

**Git Tag:** `v0.5.0`

### Objectives Achieved

* Dynamic Header architecture completed
* Header API (`setHeader()`) implemented
* Header standardised as:

  * Left = Context
  * Centre = State
  * Right = Status
* Filter Grid established as the application landing screen
* Station List and Player screens migrated to the new Header architecture
* Wi-Fi placeholder icon introduced
* First UI Theme framework implemented
* Six colour themes created
* ThemeID framework introduced
* Central Design Guide documented
* Icons module created
* Typography module created
* Project design philosophy documented

### Design System

The project now separates visual design into dedicated modules:

```text
09_UI_Theme_Design.ino
10_UI_Icons.ino
11_UI_Typography.ino
```

This establishes a reusable design system independent of the application logic.

### UI Philosophy

The project now follows the principles:

* Premium appliance, not Arduino demo.
* Typography before decoration.
* Consistent visual hierarchy.
* Header displays context only.
* Footer provides navigation only.
* Content performs the current task.
* Information appears only when it adds value.

### Status

PASS — UI Design System established.

PASS — Theme framework operational.

PASS — Project architecture prepared for production-quality typography and iconography.


## v0.7.0 – UI Polish

### Typography

* [x] Roboto typography system
* [x] Typography roles
* [x] Smooth font integration
* [x] Standard button typography

### Icons and Graphics

* [x] SVG to RAW565 asset pipeline
* [x] Dynamic Wi-Fi icons
* [x] Shared icon framework
* [x] LittleFS icon assets

### User Interface

* [x] Home Dashboard
* [x] Filter screen
* [x] Station List
* [x] Shared header framework
* [x] Theme integration
* [x] Design consistency framework

---

## v0.8.0 – Audio Engine

* [x] MAX98357A I²S amplifier integration
* [x] Reliable audio playback
* [x] Audio abstraction layer
* [x] Playback state management
* [x] Volume control
* [x] Stream recovery
* [x] BBC HLS playback
* [x] Commercial stream playback

---

## v0.8.x – Station Library

### Catalogue Architecture

* [x] Custom ESP32 partition layout
* [x] LittleFS catalogue storage
* [x] Local Station Provider
* [x] Flat paged catalogue format
* [x] Catalogue metadata
* [x] Station Provider abstraction
* [x] Local catalogue browsing

### Station Library Builder

* [x] Curated UK station library
* [x] Production catalogue generation
* [x] Automatic LittleFS deployment
* [x] Build validation
* [x] BBC catalogue provider
* [x] Runtime Radio Browser dependency removed

---

## v0.9.0 – Player Experience

* [x] Dedicated Player footer
* [x] Station artwork
* [x] Default artwork fallback
* [x] PNG artwork pipeline
* [x] Curated artwork overrides
* [x] Rounded station artwork
* [x] Live ICY metadata
* [x] Dynamic metadata updates
* [x] Theme-aware Player typography
* [x] Metadata word wrapping
* [x] Fast station switching

---

## v0.9.1 – Initial BBC Metadata

* [x] Official BBC metadata integration
* [x] BBC RMS metadata client
* [x] National BBC programme metadata
* [x] Player programme display
* [x] Programme end-time storage
* [x] Initial startup cache implementation
* [x] Git milestone tagged (`v0.9.1`)

---

## v0.9.2 – BBC Metadata and HTTPS Stability

### BBC Metadata

* [x] Replace startup cache with on-demand metadata retrieval
* [x] Request metadata only for the selected BBC station
* [x] Delay metadata request until audio playback has started
* [x] Periodic programme metadata refresh
* [x] Active BBC metadata lifecycle management
* [x] Preserve commercial ICY metadata behaviour
* [x] Maintain audio processing during BBC HTTP requests

### Audio and Memory

* [x] Diagnose TLS failures as internal-memory pressure
* [x] Reduce I²S DMA descriptors from 32 to 16
* [x] Restore reliable HTTPS stream connections
* [x] Validate BBC metadata retrieval during active playback
* [x] Add heap diagnostics for metadata and audio testing

### Startup and Repository

* [x] Branded startup splash screen
* [x] Refined startup status sequence
* [x] Package project branding assets
* [x] Package Icon Builder and generated icon assets
* [x] Merge completed milestone into `main`
* [x] Git milestone tagged (`v0.9.2`)


## v0.9.3 – Product Polish

### User Interface

* [x] Review Player layout across representative stations
* [x] Final typography review
* [x] Review alignment, spacing and visual consistency
* [x] Review touch feedback and pressed-state consistency
* [x] Replace remaining hard-coded pressed-state colours with semantic theme colours
* [x] Review screen transitions and redraw behaviour
* [x] Wi-Fi configuration

### System

* [x] Reduce remaining development Serial output
* [x] Retain useful fault and startup diagnostics
* [x] Remove obsolete code and superseded metadata-cache logic
* [x] Final code organisation review
* [x] Documentation review

### Repository

* [x] Review packaged tools for setup instructions
* [x] Review tracked assets for completeness

---

---

## v0.9.4 – Hardware Integration

### Hardware

* Custom carrier PCB designed in KiCad
* ESP32-S3 and dual MAX98357A amplifier modules integrated
* External USB-C power and firmware-upload connection
* Rear-panel power switching
* PCB manufactured and assembled
* Complete hardware validated in the finished unit

### Enclosure

* Three-part enclosure designed for 3D printing
* Separate base, main chassis and face frame
* Dual 2" speaker mounting
* M3 threaded-insert construction
* Rear USB-C and power-button access
* Finished enclosure printed, assembled and validated

### Status

PASS — Custom hardware and enclosure completed.

---

## v1.0.0 – Internet Radio

### Release Milestone

* Firmware feature set completed
* User interface and Player experience completed
* Curated UK station library completed
* BBC and commercial-station metadata operational
* Dual-mono audio system completed
* Custom PCB and enclosure completed
* Final radio assembled and operational
* Build and deployment workflow validated
* Project documentation consolidated
* Git milestone tagged (`v1.0.0`)

### Public Release Preparation

Following completion of v1.0.0, the private development repository was reviewed and consolidated in preparation for a clean public release.

This included:

* Reviewing and consolidating project documentation
* Removing obsolete development notes and diagnostic material
* Reviewing build and deployment instructions
* Reviewing packaged tools and assets
* Preparing the project for publication from a clean repository snapshot

### Status

PASS — Internet Radio v1.0.0 completed and validated.

---

## v1.0.1 – Reliability and Usability Refinement

### Wi-Fi

* Added storage for up to three Wi-Fi networks
* Added visible-network scanning at startup
* Saved networks are attempted in most-recently-used order
* Reduced connection delay when moving between known networks
* Wi-Fi reset now clears both WiFiManager credentials and the saved network list

### Station Catalogue and Streaming

* Changed the All-stations catalogue to load in pages of up to five stations
* Reduced internal heap usage and fragmentation during station browsing
* Improved reliability when connecting to HTTPS/TLS audio streams
* Refreshed the curated station catalogue
* Removed the obsolete talkSPORT 2 stream

### Player

* Refined Player metadata layout and clearing behaviour
* Improved handling of longer artist and track information
* Added automatic return to the Player after 20 seconds of touchscreen inactivity while playback remains active
* Preserved current commercial-station metadata when returning to the Player
* Preserved BBC programme metadata when returning to the Player

### Maintenance

* Removed obsolete RadioBrowser test and debug modules
* Renumbered firmware modules while preserving compilation order
* Retained useful heap, catalogue and audio connection diagnostics
* Corrected display hardware documentation
* Updated Wi-Fi, Player and typography documentation

### Status

PASS — v1.0.1 firmware refinements implemented, compiled and validated on the completed radio.
