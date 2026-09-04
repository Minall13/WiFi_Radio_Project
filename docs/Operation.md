# Wi-Fi Radio Project — Operation

This document describes normal operation of the completed Wi-Fi Radio Project.

The radio is designed to behave as a standalone appliance. Once firmware, station data and Wi-Fi credentials have been configured, no computer or external application is required for normal use.

---

## 1. Powering On

Press the rear power button to switch on the radio.

The startup screen displays progress while the radio initialises its main subsystems.

The normal startup sequence includes:

1. Initialising the local filesystem.
2. Connecting to Wi-Fi.
3. Caching BBC programme information.
4. Initialising the audio system.
5. Loading the station catalogue.

When startup is complete, the display shows:

**Ready**

followed by:

**Touch anywhere to continue**

Touch the screen to enter the radio interface.

---

## 2. Wi-Fi

The radio requires a 2.4 GHz Wi-Fi connection for Internet radio streaming and programme metadata.

Up to three Wi-Fi networks can be stored. At startup, the radio scans for visible saved networks and attempts to connect in most-recently-used order, allowing it to move between familiar locations without requiring Wi-Fi setup each time.

If no saved network is available, the Wi-Fi configuration system opens the InternetRadio-Setup portal so that a network can be selected and its credentials entered. Newly configured networks are added to the saved list and promoted to most-recently-used status.

Once successfully configured, the radio reconnects to the saved network automatically whenever it is switched on.

---

## 3. Station Filters

After the startup screen, the normal listening flow begins at the **Filter** screen.

Stations are organised into groups to make the catalogue easier to navigate.

Available groups include station families and categories such as:

- BBC
- Absolute
- Capital
- Heart
- Kiss
- Rock
- Talk
- Virgin

An **All** option provides access to the complete station catalogue.

Touch a filter to display the stations belonging to that group.

---

## 4. Station List

The Station List displays the stations available within the selected filter.

Touch a station to begin playback.

The radio then opens the **Player** screen and attempts to connect to the selected Internet stream.

Station information and artwork are loaded from the catalogue stored locally in LittleFS.

---

## 5. Player

The Player screen is the main listening interface.

It displays:

- station name;
- station artwork;
- current track information where available;
- BBC programme information where available;
- Wi-Fi status;
- playback controls.

The footer contains five controls:

| Control | Function |
| --- | --- |
| **Back** | Returns to the Station List while the current station continues playing |
| **Stop** | Stops playback and returns to the Filter screen |
| **Volume −** | Reduces playback volume |
| **MUTE** | Toggles latching mute without stopping the stream |
| **Volume +** | Increases playback volume |

---

## 6. Back and Stop Behaviour

**Back** and **Stop** intentionally perform different functions.

### Back

Touching **Back** returns to the current Station List while the audio stream continues playing.

This allows another station to be selected without first stopping the current one.

While a station is playing, the radio automatically returns to the Player screen after 20 seconds without touchscreen activity. Touching the screen resets this inactivity period, allowing the Station List and other screens to be browsed for as long as required.

### Stop

Touching **Stop** ends the current audio stream and returns to the Filter screen.

The Filter screen represents the beginning of the normal listening flow:

```text
Filter
  ↓
Station List
  ↓
Player
```

---

## 7. Volume

Use **Volume −** and **Volume +** to adjust playback level.

The project firmware limits the normal volume range to values suitable for the MAX98357A amplifier modules and speakers used in the original build.

Very high digital gain levels can cause audible distortion and are intentionally avoided.

---

## 8. Mute

**MUTE** is a latching control.

When mute is enabled:

- audio output is silenced;
- the Internet stream continues;
- playback position is not intentionally interrupted.

Touch **MUTE** again to restore the previous listening state.

Mute is therefore different from **Stop**, which terminates the current stream.

---

## 9. Programme and Track Information

The radio supports two principal forms of live metadata.

### Commercial Stations

Where provided by the broadcaster, ICY metadata is extracted from the audio stream and displayed on the Player screen.

This commonly contains artist and track information.

Metadata availability and formatting are controlled by the broadcaster and therefore vary between stations.

### BBC Stations

BBC stations use programme metadata obtained separately from the audio stream.

This allows the Player screen to display the programme currently being broadcast.

---

## 10. Station Artwork

Station artwork is displayed on the Player screen where suitable artwork is available.

Artwork is stored locally in LittleFS rather than downloaded each time a station is selected.

If no suitable artwork is available, the project can use default station artwork generated by the Station Library Builder.

---

## 11. Settings / System Information

The radio includes a system information/settings screen containing information about the running unit and its network connection.

The displayed information includes the current Wi-Fi connection and firmware identification.

Long Wi-Fi SSIDs are shortened on screen to fit the fixed display layout.

---

## 12. Changing Station

There are two normal ways to change station.

### Within the same group

From the Player screen:

1. Touch **Back**.
2. The current station continues playing.
3. Select another station from the Station List.
4. The Player opens for the newly selected station.

### From another group

From the Player screen:

1. Touch **Stop**.
2. Playback ends.
3. The Filter screen opens.
4. Select another filter.
5. Select the required station.

---

## 13. Switching Off

The radio may be switched off using the rear power button.

There is no software shutdown sequence required for normal use.

---

## 14. Internet Stream Availability

Internet radio streams are operated by their respective broadcasters.

A station that previously worked may occasionally become unavailable because of:

- broadcaster maintenance;
- changes to stream URLs;
- geographic restrictions;
- changes to streaming format;
- temporary Internet or server problems.

Such changes can occur independently of the Wi-Fi Radio Project.

The local station catalogue can be rebuilt or updated when station streams change.

---

## 15. Normal Listening Flow

In everyday use, operation is intentionally simple:

```text
Power On
   ↓
Startup
   ↓
Touch to Continue
   ↓
Filter
   ↓
Station List
   ↓
Player
```

From the Player:

```text
Back → Station List
       Audio continues

Stop → Filter
       Audio stops
```

The intention is that normal listening requires no knowledge of the underlying ESP32 firmware, station catalogue or streaming architecture.