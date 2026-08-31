// ============================================================
// 14_Station_List.ino
// Station List Screen
// ============================================================
//
// Purpose:
//
// Implements the station browsing screen.
//
// Responsibilities:
//
//   • Display the current station catalogue.
//   • Support page navigation.
//   • Highlight the selected station.
//   • Launch the Player screen when a station is chosen.
//
// Station data is provided by StationProvider.
// Playback is handled by the Player and Audio Engine.
//
// ============================================================


void drawStationsScreen() {
  currentScreen = SCREEN_STATIONS;

  int currentPage = (stationPageStart / STATIONS_PER_PAGE) + 1;
  int totalPages  = (stationCount + STATIONS_PER_PAGE - 1) / STATIONS_PER_PAGE;

  setHeader(stationFilters[currentStationFilter],
            "Page " + String(currentPage) + " of " + String(totalPages),
            HEADER_STATUS_WIFI);

  safeTftStart();

  drawScreenFramework();

  uiApplyTypography(TYPE_BODY, theme.textPrimary);

  for (int i = 0; i < STATIONS_PER_PAGE; i++) {
    int stationIndex = stationPageStart + i;

    if (stationIndex >= stationCount) {
      break;
    }

    tft.setCursor(25, 72 + (i * 33));
    tft.print(i + 1);
    tft.print(". ");

    String displayName = stations[stationIndex].name;

    const int maxStationTextWidth = 305;

    while (tft.textWidth(displayName) > maxStationTextWidth && displayName.length() > 4) {
      displayName.remove(displayName.length() - 1);
    }

    if (displayName != stations[stationIndex].name) {
      displayName += "...";
    }

    tft.println(displayName);
  }

  tft.setTextFont(1);

  safeTftEnd();

  drawArrowButton(stationPrevButton, false, true);
  drawArrowButton(stationNextButton, false, false);
}