// ============================================================
// 03_Touch_Input.ino
// Touch Input Handling
// ============================================================
//
// Purpose:
//
// Reads the XPT2046 touch controller and converts touch events
// into application input.
//
// Responsibilities:
//
//   • Read touch coordinates.
//   • Apply screen calibration.
//   • Detect button presses.
//   • Route touch input to the active screen.
//
// This module contains touch input only. It does not draw the UI.
//
// ============================================================

bool splashAwaitingTouch = true;

// ============================================================
// Station Selection Helper
// ============================================================

void selectStationFromFooter(int footerButtonNumber) {
  int selectedIndex =
    stationPageStart + (footerButtonNumber - 1);

  if (selectedIndex >= stationCount) {
    Serial.println(
      "Footer station button ignored: no station on this button"
    );
    return;
  }

  selectedStationIndex = selectedIndex;

  Serial.println("Station selected from footer:");

  Serial.print("Index: ");
  Serial.println(selectedStationIndex);

  Serial.print("Name: ");
  Serial.println(stations[selectedStationIndex].name);

  Serial.print("URL: ");
  Serial.println(stations[selectedStationIndex].streamUrl);

  showMessage(
    stations[selectedStationIndex].name.c_str()
  );

  drawPlayerScreen();

  audioPlay(
    stations[selectedStationIndex].streamUrl
  );

  if (
    stations[selectedStationIndex].metadataService.length() > 0
  ) {
    bbcMetadataBegin(
      stations[selectedStationIndex].metadataService
    );
  } else {
    bbcMetadataStop();
  }
}


// ============================================================
// Screen Navigation Helper
// ============================================================

void navigateBack() {
  if (currentScreen == SCREEN_PLAYER) {
    drawStationsScreen();
    return;
  }

  if (currentScreen == SCREEN_STATIONS) {
    drawFilterScreen();
    return;
  }

  if (currentScreen == SCREEN_FILTERS) {
    drawHomeScreen();
    return;
  }

  drawHomeScreen();
}


// ============================================================
// Settings Modal
// ============================================================

void showSettingsModal() {
  Serial.println("Settings modal placeholder");

  safeTftStart();

  int modalX = 60;
  int modalY = 80;
  int modalW = 360;
  int modalH = 150;

  tft.fillRoundRect(
    modalX,
    modalY,
    modalW,
    modalH,
    12,
    theme.panel
  );

  tft.drawRoundRect(
    modalX,
    modalY,
    modalW,
    modalH,
    12,
    theme.buttonBorder
  );

  tft.setTextColor(
    theme.textPrimary,
    theme.panel
  );

  tft.setTextSize(3);
  tft.setCursor(
    modalX + 85,
    modalY + 35
  );

  tft.print("Settings");

  tft.setTextColor(
    theme.accent,
    theme.panel
  );

  tft.setTextSize(2);
  tft.setCursor(
    modalX + 75,
    modalY + 90
  );

  tft.print("Coming soon");

  safeTftEnd();
}

// ============================================================
// Home Screen Touch Handler
// ============================================================

bool handleHomeScreenTouch(int screenX, int screenY) {

  // Open station filter screen
  if (insideButton(
        screenX,
        screenY,
        stationsButton
      )) {

    drawButton(stationsButton, true);
    delay(150);
    drawButton(stationsButton, false);

    drawFilterScreen();
    return true;
  }

  // WiFi reset / setup
  if (insideButton(
        screenX,
        screenY,
        wifiSetupButton
      )) {

    if (!confirmWiFiReset) {
      confirmWiFiReset = true;
      confirmWiFiResetStart = millis();

      handleButtonPress(
        wifiSetupButton,
        "Press again to reset"
      );
    }
    else {
      confirmWiFiReset = false;

      handleButtonPress(
        wifiSetupButton,
        "Reset WiFi"
      );

      resetWiFiSettings();
    }

    return true;
  }

  return false;
}

// ============================================================
// Filter Screen Touch Handler
// ============================================================

bool handleFilterScreenTouch(int screenX, int screenY) {

  // ----------------------------------------------------------
  // Settings Button
  // ----------------------------------------------------------

  if (insideButton(
        screenX,
        screenY,
        filterSettingsButton
      )) {

    drawFilterSettingsButton(true);
    delay(150);
    drawFilterSettingsButton(false);

    drawHomeScreen();

    return true;
  }

  // ----------------------------------------------------------
  // Filter Selection
  // ----------------------------------------------------------

  int selectedFilter = -1;
  Button selectedButton;

  if (insideButton(
        screenX,
        screenY,
        filterBBCButton
      )) {

    selectedFilter = 0;
    selectedButton = filterBBCButton;
  }

  else if (insideButton(
             screenX,
             screenY,
             filterAbsoluteButton
           )) {

    selectedFilter = 1;
    selectedButton = filterAbsoluteButton;
  }

  else if (insideButton(
             screenX,
             screenY,
             filterVirginButton
           )) {

    selectedFilter = 2;
    selectedButton = filterVirginButton;
  }

  else if (insideButton(
             screenX,
             screenY,
             filterCapitalButton
           )) {

    selectedFilter = 3;
    selectedButton = filterCapitalButton;
  }

  else if (insideButton(
             screenX,
             screenY,
             filterHeartButton
           )) {

    selectedFilter = 4;
    selectedButton = filterHeartButton;
  }

  else if (insideButton(
             screenX,
             screenY,
             filterKissButton
           )) {

    selectedFilter = 5;
    selectedButton = filterKissButton;
  }

  else if (insideButton(
             screenX,
             screenY,
             filterRockButton
           )) {

    selectedFilter = 6;
    selectedButton = filterRockButton;
  }

  else if (insideButton(
             screenX,
             screenY,
             filterTalkButton
           )) {

    selectedFilter = 7;
    selectedButton = filterTalkButton;
  }

  else if (insideButton(
             screenX,
             screenY,
             filterAllButton
           )) {

    selectedFilter = 8;
    selectedButton = filterAllButton;
  }

  if (selectedFilter < 0) {
    return false;
  }

  drawFilterChoiceButton(
    selectedButton,
    true
  );

  delay(150);

  currentStationFilter = selectedFilter;
  stationPageStart = 0;

  if (!stationProviderLoad(
        stationFilters[currentStationFilter]
      )) {

    showMessage("Catalogue load failed");
    return true;
  }

  drawStationsScreen();

  return true;
}

// ============================================================
// Station List Screen Touch Handler
// ============================================================

bool handleStationListTouch(int screenX, int screenY) {

  // Return to Filter screen
  if (insideButton(
        screenX,
        screenY,
        filterMenuButton
      )) {

    drawFilterMenuButton(true);
    delay(150);
    drawFilterMenuButton(false);

    drawFilterScreen();
    return true;
  }

  // Footer Back button
  if (insideButton(
        screenX,
        screenY,
        backFooterButton
      )) {

    drawFooterBackButton(true);
    delay(150);
    drawFooterBackButton(false);

    navigateBack();
    return true;
  }

  // Footer Settings button
  if (insideButton(
        screenX,
        screenY,
        settingsFooterButton
      )) {

    drawFooterSettingsButton(true);
    delay(150);
    drawFooterSettingsButton(false);

    drawHomeScreen();

    return true;
  }

  // Previous station page
  if (insideButton(
        screenX,
        screenY,
        stationPrevButton
      )) {

    drawArrowButton(
      stationPrevButton,
      true,
      true
    );

    delay(150);

    drawArrowButton(
      stationPrevButton,
      false,
      true
    );

    stationPageStart -= STATIONS_PER_PAGE;

    if (stationPageStart < 0) {
      int lastPageStart =
        ((stationCount - 1) /
         STATIONS_PER_PAGE) *
        STATIONS_PER_PAGE;

      stationPageStart = lastPageStart;
    }

    drawStationsScreen();
    return true;
  }

  // Next station page
  if (insideButton(
        screenX,
        screenY,
        stationNextButton
      )) {

    drawArrowButton(
      stationNextButton,
      true,
      false
    );

    delay(150);

    drawArrowButton(
      stationNextButton,
      false,
      false
    );

    stationPageStart += STATIONS_PER_PAGE;

    if (stationPageStart >= stationCount) {
      stationPageStart = 0;
    }

    drawStationsScreen();
    return true;
  }

  // Station selection buttons
  if (insideButton(
        screenX,
        screenY,
        stationSelectButton1
      )) {

    selectStationFromFooter(1);
    return true;
  }

  if (insideButton(
        screenX,
        screenY,
        stationSelectButton2
      )) {

    selectStationFromFooter(2);
    return true;
  }

  if (insideButton(
        screenX,
        screenY,
        stationSelectButton3
      )) {

    selectStationFromFooter(3);
    return true;
  }

  if (insideButton(
        screenX,
        screenY,
        stationSelectButton4
      )) {

    selectStationFromFooter(4);
    return true;
  }

  if (insideButton(
        screenX,
        screenY,
        stationSelectButton5
      )) {

    selectStationFromFooter(5);
    return true;
  }

  return false;
}

// ============================================================
// Player Screen Touch Handler
// ============================================================

bool handlePlayerScreenTouch(int screenX, int screenY) {
  static int volumeBeforeMute = 14;
  static bool playerMuted = false;

  // Return to Station List while playback continues
  if (insideButton(
        screenX,
        screenY,
        playerBackButton
      )) {

    drawPlayerSymbolButton(
      playerBackButton,
      theme.playerBack,
      true,
      PLAYER_SYMBOL_BACK
    );

    delay(150);

    navigateBack();

    return true;
  }

  // Stop playback and return to Filter screen
  if (insideButton(
        screenX,
        screenY,
        playerStopButton
      )) {

    drawPlayerSymbolButton(
      playerStopButton,
      theme.playerStop,
      true,
      PLAYER_SYMBOL_STOP
    );

    delay(150);

    audioStop();

    drawFilterScreen();

    return true;
  }

  // Volume down
  if (insideButton(
        screenX,
        screenY,
        playerVolumeDownButton
      )) {

    int newVolume =
      constrain(audioGetVolume() - 1, 0, 21);

    audioSetVolume(newVolume);

    playerMuted = false;

    drawPlayerSymbolButton(
      playerVolumeDownButton,
      theme.playerVolumeDown,
      true,
      PLAYER_SYMBOL_DOWN
    );

    delay(150);

    drawPlayerSymbolButton(
      playerVolumeDownButton,
      theme.playerVolumeDown,
      false,
      PLAYER_SYMBOL_DOWN
    );

    return true;
  }

  // Mute / restore volume
  if (insideButton(
        screenX,
        screenY,
        playerMuteButton
      )) {

    if (!playerMuted) {
      volumeBeforeMute = audioGetVolume();
      audioSetVolume(0);
      playerMuted = true;
    }
    else {
      audioSetVolume(volumeBeforeMute);
      playerMuted = false;
    }

    drawPlayerControlButton(
      playerMuteButton,
      "MUTE",
      theme.playerMute,
      playerMuted
    );

    return true;
  }

  // Volume up
  if (insideButton(
        screenX,
        screenY,
        playerVolumeUpButton
      )) {

    int newVolume =
      constrain(audioGetVolume() + 1, 0, 21);

    audioSetVolume(newVolume);

    playerMuted = false;

    drawPlayerSymbolButton(
      playerVolumeUpButton,
      theme.playerVolumeUp,
      true,
      PLAYER_SYMBOL_UP
    );

    delay(150);

    drawPlayerSymbolButton(
      playerVolumeUpButton,
      theme.playerVolumeUp,
      false,
      PLAYER_SYMBOL_UP
    );

    return true;
  }

  return false;
}


// ============================================================
// Main Loop / Touch Dispatcher
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // Audio Engine
  // ----------------------------------------------------------

  audioLoop();

  // ----------------------------------------------------------
  // Player Metadata Update
  // ----------------------------------------------------------

  bbcMetadataLoop();

  if (
    currentScreen == SCREEN_PLAYER &&
    (
      streamTitleChanged ||
      bbcMetadataChanged
    )
  ) {
    drawPlayerMetadataArea();

    streamTitleChanged = false;
    bbcMetadataChanged = false;
  }

  // ----------------------------------------------------------
  // Touch Input
  // ----------------------------------------------------------

  if (ts.touched()) {

    TS_Point p = ts.getPoint();

    int screenX =
      map(
        p.x,
        TOUCH_MIN_X,
        TOUCH_MAX_X,
        0,
        480
      );

    int screenY =
      map(
        p.y,
        TOUCH_MIN_Y,
        TOUCH_MAX_Y,
        0,
        320
      );

    screenX = constrain(
      screenX,
      0,
      479
    );

    screenY = constrain(
      screenY,
      0,
      319
    );

    Serial.print("Raw X=");
    Serial.print(p.x);
    Serial.print(" Y=");
    Serial.print(p.y);
    Serial.print(" -> Screen X=");
    Serial.print(screenX);
    Serial.print(" Y=");
    Serial.println(screenY);

    // --------------------------------------------------------
    // Splash Screen Touch Gate
    // --------------------------------------------------------

    if (splashAwaitingTouch) {

      splashAwaitingTouch = false;

      // Consume the complete touch before drawing the Home
      // screen, preventing the same press from activating an
      // underlying Home-screen control.
      while (ts.touched()) {
        audioLoop();
        delay(1);
      }

      drawFilterScreen();

      return;
    }

    // --------------------------------------------------------
    // Dispatch Touch According to Current Screen
    // --------------------------------------------------------

    switch (currentScreen) {

      // ------------------------------------------------------
      // Home Screen
      // ------------------------------------------------------

      case SCREEN_HOME:
        handleHomeScreenTouch(
          screenX,
          screenY
        );
        break;

      // ------------------------------------------------------
      // Filter Screen
      // ------------------------------------------------------

      case SCREEN_FILTERS:
        handleFilterScreenTouch(
          screenX,
          screenY
        );
        break;

      // ------------------------------------------------------
      // Station List Screen
      // ------------------------------------------------------

      case SCREEN_STATIONS:
        handleStationListTouch(
          screenX,
          screenY
        );
        break;

      // ------------------------------------------------------
      // Player Screen
      // ------------------------------------------------------

      case SCREEN_PLAYER:
        handlePlayerScreenTouch(
          screenX,
          screenY
        );
        break;
    }
  }

  // ----------------------------------------------------------
  // Deferred UI Messages
  // ----------------------------------------------------------

  if (messagePending) {

    showMessage(
      pendingMessage.c_str()
    );

    messagePending = false;
  }

  // ----------------------------------------------------------
  // WiFi Reset Confirmation Timeout
  // ----------------------------------------------------------

  if (
    confirmWiFiReset &&
    millis() - confirmWiFiResetStart > 5000
  ) {

    confirmWiFiReset = false;

    showMessage("Reset cancelled");
    delay(1000);

    String wifiStatus =
      WiFi.SSID() +
      " " +
      WiFi.localIP().toString();

    showMessage(
      wifiStatus.c_str()
    );
  }

  // ----------------------------------------------------------
  // Loop Throttle
  // ----------------------------------------------------------

  delay(1);
}