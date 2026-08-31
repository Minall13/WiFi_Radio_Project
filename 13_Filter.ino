// ============================================================
// 13_Filter.ino
// Station Filter Screen
// ============================================================
//
// Purpose:
//
// Implements the station filter selection screen.
//
// Responsibilities:
//
//   • Display available station categories.
//   • Allow the user to select a station filter.
//   • Initiate loading of the selected station catalogue.
//
// Station data is provided by StationProvider.
// Screen navigation is handled by the application.
//
// ============================================================


void drawFilterScreen() {
  currentScreen = SCREEN_FILTERS;

  setHeader(
    "Filters",
    "Choose Station Group",
    HEADER_STATUS_WIFI
  );

  safeTftStart();

  tft.fillScreen(theme.bg);

  safeTftEnd();

  drawFilterChoiceButton(
    filterBBCButton,
    currentStationFilter == 0
  );

  drawFilterChoiceButton(
    filterAbsoluteButton,
    currentStationFilter == 1
  );

  drawFilterChoiceButton(
    filterVirginButton,
    currentStationFilter == 2
  );

  drawFilterChoiceButton(
    filterCapitalButton,
    currentStationFilter == 3
  );

  drawFilterChoiceButton(
    filterHeartButton,
    currentStationFilter == 4
  );

  drawFilterChoiceButton(
    filterKissButton,
    currentStationFilter == 5
  );

  drawFilterChoiceButton(
    filterRockButton,
    currentStationFilter == 6
  );

  drawFilterChoiceButton(
    filterTalkButton,
    currentStationFilter == 7
  );

  drawFilterChoiceButton(
    filterAllButton,
    currentStationFilter == 8
  );

  drawFilterSettingsButton(false);
}