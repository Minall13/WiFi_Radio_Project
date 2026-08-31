# Wi-Fi Radio Project — Audio

## Overview

The Wi-Fi Radio Project provides Internet radio playback using the ESP32-S3, the ESP32-audioI2S library and two MAX98357A I²S amplifier modules.

The audio subsystem supports the stream formats used by the curated station library, including MP3, AAC/AAC+, HTTP, HTTPS and BBC HLS streams.

Audio processing is intentionally isolated from the user interface and station catalogue.

---

## Hardware

### Controller

ESP32-S3 N16R8

### Amplifiers

2 × MAX98357A I²S Class-D amplifier modules.

Both amplifiers receive the same I²S audio data and drive separate speakers, providing a dual-mono output.

### Speakers

2 × 3 W, 4 Ω speakers.

---

## I²S Configuration

| Signal | GPIO |
|--------|------|
| BCLK | GPIO6 |
| LRCK | GPIO7 |
| DOUT | GPIO5 |

The two MAX98357A modules share the I²S signals.

The known working module configuration is:

| Pin | Connection |
|-----|------------|
| VIN | 5 V |
| GND | GND |
| DIN | GPIO5 |
| BCLK | GPIO6 |
| LRC | GPIO7 |
| SD | 3.3 V |
| GAIN | Floating |

---

## Audio Library

The project uses **ESP32-audioI2S**.

The library is wrapped by the project's Audio Engine rather than being accessed directly by the user interface.

The principal interface includes:

```cpp
audioBegin()
audioPlay()
audioStop()
audioLoop()
audioSetVolume()
audioGetVolume()
```

This abstraction keeps playback implementation separate from UI and station-catalogue logic.

---

## Volume and Mute

The normal default volume is:

```text
14
```

This provides clean playback with the amplifier and speaker combination used in the finished radio.

Mute is implemented as a latching control. Muting silences the output without terminating the active stream, allowing playback to resume immediately when mute is released.

---

## DMA Configuration

An important optimisation made during development concerns the audio DMA configuration.

HTTPS connection failures were traced to **internal RAM pressure**, rather than to TLS itself.

The ESP32-audioI2S default DMA configuration allocated more internal memory than this application required.

The project therefore uses:

```cpp
audio.settings.DMA_DESC_NUM = 16;
```

rather than the library default of 32 descriptors.

This change:

- increases available internal heap;
- increases the largest available internal memory block;
- improves reliability when establishing HTTPS streams;
- leaves sufficient memory for metadata operations during playback;
- produces no observable reduction in playback quality.

This setting forms part of the project's known working audio configuration.

---

## Main Loop Requirements

The audio library requires:

```cpp
audioLoop();
```

to be serviced frequently.

Long blocking operations can result in buffer underruns, interrupted audio, slower reconnects or delayed metadata processing.

During development, a long main-loop delay was found to cause periodic audio pulsing. Reducing the delay to approximately 1 ms restored stable playback.

Code added to the main application loop should therefore avoid unnecessary blocking operations.

---

## Metadata

### Commercial Stations

Where supported by the station stream, ICY metadata provides current artist and track information.

Metadata updates are handled without requiring the complete Player screen to be redrawn.

### BBC Stations

BBC programme information is obtained separately from the audio stream through the project's BBC metadata subsystem.

This allows current BBC programme information to be presented through the Player interface even though it is supplied through a different mechanism from commercial ICY metadata.

---

## Playback Behaviour

The Audio Engine maintains playback independently from screen navigation.

This allows the current stream to continue when the user selects **Back** from the Player and returns to the Station List.

Selecting **Stop** explicitly terminates the current stream.

Volume and mute operations alter the audio output without changing the selected station or navigation state.

---

## Design Constraints

Changes to the audio subsystem should preserve the following known working characteristics:

- I²S on GPIO5, GPIO6 and GPIO7;
- dual-mono MAX98357A output;
- frequent servicing of `audioLoop()`;
- minimal blocking activity during playback;
- reduced DMA descriptor allocation;
- sufficient internal RAM for HTTPS connections and metadata operations.

These constraints were established through testing and form part of the stable v1.0 audio implementation.