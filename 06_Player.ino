// ============================================================
// 06_Player.ino
// Player Screen
// ============================================================
//
// Purpose:
//
// Implements the Internet Radio player screen and playback UI.
//
// Responsibilities:
//
//   • Display station artwork and information.
//   • Display live stream and BBC programme metadata.
//   • Manage playback controls and volume.
//   • Update the screen in response to player state changes.
//
// Audio playback is handled by AudioEngine.
// Station selection is handled by StationProvider.
//
// ============================================================



// ============================================================
// Player UI Constants
// ============================================================

const int PLAYER_ICON_SIZE     = 52;
const int PLAYER_ICON_X_OFFSET = -1;
const int PLAYER_ICON_Y_OFFSET = -2;

// ============================================================
// Player Artwork
// ============================================================

const int PLAYER_LOGO_SIZE = 190;

const int PLAYER_LOGO_X = 265;
const int PLAYER_LOGO_Y = 60;

// ============================================================
// Player Content Text
// ============================================================

const int PLAYER_TEXT_X = 25;
const int PLAYER_TEXT_Y = 94;
const int PLAYER_TEXT_GAP = 5;
const int PLAYER_TEXT_LINE_HEIGHT = 32;

// ============================================================
// Player Screen - Main
// ============================================================

void drawPlayerStationName(const String& stationName) {
  const int maximumWidth =
    PLAYER_LOGO_X - PLAYER_TEXT_X - PLAYER_TEXT_GAP;

  String firstLine = "";
  String secondLine = "";
  int wordStart = 0;

  while (wordStart < stationName.length()) {
    int spacePosition = stationName.indexOf(' ', wordStart);

    String word;

    if (spacePosition < 0) {
      word = stationName.substring(wordStart);
      wordStart = stationName.length();
    }
    else {
      word = stationName.substring(wordStart, spacePosition);
      wordStart = spacePosition + 1;
    }

    String candidate =
      firstLine.length() > 0
        ? firstLine + " " + word
        : word;

    if (tft.textWidth(candidate) <= maximumWidth) {
      firstLine = candidate;
    }
    else {
      secondLine =
        secondLine.length() > 0
          ? secondLine + " " + word
          : word;
    }
  }

  uiApplyTypography(TYPE_BODY, theme.textStrong);

  tft.setCursor(
    PLAYER_TEXT_X,
    PLAYER_TEXT_Y
  );
  tft.print(firstLine);

  if (secondLine.length() > 0) {
    tft.setCursor(
      PLAYER_TEXT_X,
      PLAYER_TEXT_Y + PLAYER_TEXT_LINE_HEIGHT
    );
    tft.print(secondLine);
  }
}

// ============================================================
// Player Stream Metadata
// ============================================================

void drawPlayerStreamTitle(const String& streamTitle) {
  bool isBBC =
    selectedStationIndex >= 0 &&
    selectedStationIndex < stationCount &&
    stations[selectedStationIndex].metadataService.length() > 0;

  String metadata = streamTitle;

  if (isBBC) {
    metadata = currentBBCProgramme;
  }

  metadata.trim();

  if (metadata.length() == 0) {
    return;
  }

  String artist = metadata;
  String track = "";

  if (!isBBC) {
    int separatorPosition = metadata.indexOf(" - ");

    if (separatorPosition >= 0) {
      artist = metadata.substring(0, separatorPosition);
      track = metadata.substring(separatorPosition + 3);
    }
  }

  const int maximumWidth =
    PLAYER_LOGO_X - PLAYER_TEXT_X - PLAYER_TEXT_GAP;

  const int lineHeight = 22;

  auto wrapText = [&](
    const String& text,
    String lines[],
    int maximumLines
  ) {
    for (int i = 0; i < maximumLines; i++) {
      lines[i] = "";
    }

    String remaining = text;
    remaining.trim();

    for (
      int lineIndex = 0;
      lineIndex < maximumLines &&
      remaining.length() > 0;
      lineIndex++
    ) {
      String line = remaining;

      while (
        line.length() > 0 &&
        tft.textWidth(line) > maximumWidth
      ) {
        int lastSpace = line.lastIndexOf(' ');

        if (lastSpace < 0) {
          line.remove(line.length() - 1);
        }
        else {
          line = line.substring(0, lastSpace);
        }
      }

      line.trim();

      if (line.length() == 0) {
        break;
      }

      lines[lineIndex] = line;

      remaining.remove(0, line.length());
      remaining.trim();

      if (
        lineIndex == maximumLines - 1 &&
        remaining.length() > 0
      ) {
        String truncated = lines[lineIndex] + "...";

        while (
          truncated.length() > 3 &&
          tft.textWidth(truncated) > maximumWidth
        ) {
          truncated.remove(truncated.length() - 4);
          truncated += "...";
        }

        lines[lineIndex] = truncated;
      }
    }
  };

  tft.setTextSize(2);

  if (isBBC) {
    String programmeLines[3];

    wrapText(
      artist,
      programmeLines,
      3
    );

    tft.setTextColor(
      TFT_LIGHTGREY,
      theme.bg
    );

    for (int i = 0; i < 3; i++) {
      if (programmeLines[i].length() == 0) {
        continue;
      }

      tft.setCursor(
        PLAYER_TEXT_X,
        167 + (i * lineHeight)
      );

      tft.print(programmeLines[i]);
    }

    return;
  }

  String artistLines[2];
  String trackLines[2];

  wrapText(
    artist,
    artistLines,
    2
  );

  wrapText(
    track,
    trackLines,
    2
  );

  tft.setTextColor(
    TFT_LIGHTGREY,
    theme.bg
  );

  int currentY = 167;

  for (int i = 0; i < 2; i++) {
    if (artistLines[i].length() == 0) {
      continue;
    }

    tft.setCursor(
      PLAYER_TEXT_X,
      currentY
    );

    tft.print(artistLines[i]);

    currentY += lineHeight;
  }

  tft.setTextColor(
    TFT_CYAN,
    theme.bg
  );

  for (int i = 0; i < 2; i++) {
    if (trackLines[i].length() == 0) {
      continue;
    }

    tft.setCursor(
      PLAYER_TEXT_X,
      currentY
    );

    tft.print(trackLines[i]);

    currentY += lineHeight;
  }
}

// ============================================================
// Player Stream Metadata Update
// ============================================================

void drawPlayerMetadataArea() {
  safeTftStart();

  tft.fillRect(
    PLAYER_TEXT_X,
    162,
    PLAYER_LOGO_X - PLAYER_TEXT_X - PLAYER_TEXT_GAP,
    83,
    theme.bg
  );

  drawPlayerStreamTitle(currentStreamTitle);

  safeTftEnd();
}


void drawPlayerScreen() {
  currentScreen = SCREEN_PLAYER;

  if (selectedStationIndex >= 0 && selectedStationIndex < stationCount) {
    setHeader(
      "Playing",
      stationFilters[currentStationFilter],
      HEADER_STATUS_WIFI
    );
  }
  else {
    setHeader(
      "Player",
      "No Station",
      HEADER_STATUS_WIFI
    );
  }

  safeTftStart();

  drawHeader();
  drawContentArea();
  drawFooter();
  drawPlayerFooterLayout();

  tft.setTextFont(1);
  tft.setTextSize(1);

  if (selectedStationIndex >= 0 && selectedStationIndex < stationCount) {
    drawStationArtwork(
      stations[selectedStationIndex].logo.c_str(),
      PLAYER_LOGO_X,
      PLAYER_LOGO_Y,
      theme.bg
    );

    drawPlayerStationName(
      stations[selectedStationIndex].name
    );
  }
  else {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(25, 115);
    tft.print("No station selected");
  }

  safeTftEnd();

  if (
    selectedStationIndex >= 0 &&
    selectedStationIndex < stationCount &&
    stations[selectedStationIndex].metadataService.length() > 0 &&
    currentBBCProgramme.length() > 0
  ) {
    drawPlayerMetadataArea();
  }
}

// ============================================================
// Player Screen - Footer Button Control
// ============================================================

void drawPlayerControlButton(
  Button b,
  const char* label,
  uint16_t accentColour,
  bool active
) {
  safeTftStart();

  uint16_t fillColour   = active ? accentColour : theme.playerButtonBg;
  uint16_t borderColour = accentColour;
  uint16_t textColour   = active ? TFT_BLACK : accentColour;

  tft.fillRoundRect(b.x, b.y, b.w, b.h, 10, fillColour);

  tft.drawRoundRect(b.x, b.y, b.w, b.h, 10, borderColour);
  tft.drawRoundRect(
    b.x + 1,
    b.y + 1,
    b.w - 2,
    b.h - 2,
    9,
    borderColour
  );

  if (strcmp(label, "_") == 0) {
    uiApplyTypography(TYPE_HEADER, textColour);
  }
  else if (strcmp(label, "+") == 0) {
    uiApplyTypography(TYPE_HEADER, textColour);
  }
  else {
    uiApplyTypography(TYPE_BUTTON, textColour);
  }

  tft.setTextDatum(MC_DATUM);

  int textX = b.x + (b.w / 2);
  int textY = b.y + (b.h / 2);

  if (strcmp(label, "_") == 0) {
    textY -= 2;
  }

  if (strcmp(label, "MUTE") == 0) {
    textX -= 2;
    textY += 1;
  }

  tft.drawString(label, textX, textY);
  tft.setTextDatum(TL_DATUM);

  safeTftEnd();
}

// ============================================================
// Player Screen - Symbol Button Control
// ============================================================

void drawPlayerSymbolButton(
  Button b,
  uint16_t accentColour,
  bool active,
  PlayerSymbol symbol
) {
  drawPlayerControlButton(
    b,
    "",
    accentColour,
    active
  );

  uint16_t symbolColour = active ? TFT_BLACK : accentColour;

  int centreX = b.x + (b.w / 2);
  int centreY = b.y + (b.h / 2);

  safeTftStart();

  switch (symbol) {

    case PLAYER_SYMBOL_BACK:
      tft.fillTriangle(
        centreX - 11,
        centreY,
        centreX + 8,
        centreY - 12,
        centreX + 8,
        centreY + 12,
        symbolColour
      );
      break;

    case PLAYER_SYMBOL_STOP:
      tft.fillRect(
        centreX - 11,
        centreY - 11,
        22,
        22,
        symbolColour
      );
      break;

    case PLAYER_SYMBOL_DOWN:
      tft.fillTriangle(
        centreX - 12,
        centreY - 7,
        centreX + 12,
        centreY - 7,
        centreX,
        centreY + 10,
        symbolColour
      );
      break;

    case PLAYER_SYMBOL_UP:
      tft.fillTriangle(
        centreX,
        centreY - 10,
        centreX - 12,
        centreY + 7,
        centreX + 12,
        centreY + 7,
        symbolColour
      );
      break;
  }

  safeTftEnd();
}

// ============================================================
// Player Screen - Footer
// ============================================================

void drawPlayerFooterLayout() {

  // Button Frames

  drawPlayerControlButton(
    playerBackButton,
    "",
    theme.playerBack,
    false
  );

  drawPlayerControlButton(
    playerStopButton,
    "",
    theme.playerStop,
    false
  );

  drawPlayerSymbolButton(
    playerVolumeDownButton,
    theme.playerVolumeDown,
    false,
    PLAYER_SYMBOL_DOWN
  );

  drawPlayerControlButton(
    playerMuteButton,
    "MUTE",
    theme.playerMute,
    false
  );

  drawPlayerSymbolButton(
    playerVolumeUpButton,
    theme.playerVolumeUp,
    false,
    PLAYER_SYMBOL_UP
  );

  // Back Symbol

  int backCentreX = playerBackButton.x + (playerBackButton.w / 2);
  int backCentreY = playerBackButton.y + (playerBackButton.h / 2);

  drawPlayerSymbolButton(
    playerBackButton,
    theme.playerBack,
    false,
    PLAYER_SYMBOL_BACK
  );

  // Stop Symbol

  int stopCentreX = playerStopButton.x + (playerStopButton.w / 2);
  int stopCentreY = playerStopButton.y + (playerStopButton.h / 2);

  drawPlayerSymbolButton(
    playerStopButton,
    theme.playerStop,
    false,
    PLAYER_SYMBOL_STOP
  );
}