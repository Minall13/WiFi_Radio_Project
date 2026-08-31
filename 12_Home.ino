// ============================================================
// 12_Home.ino
// Home Screen
// ============================================================
//
// Purpose:
//
// Implements the application's Home screen.
//
// Responsibilities:
//
//   • Draw the Home screen layout.
//   • Display the primary navigation controls.
//   • Handle Home screen state and visual updates.
//
// Touch input is routed by Touch_Input.
// Shared styling is provided by the UI modules.
//
// ============================================================


void drawHomeScreen() {
  safeTftStart();

  tft.fillScreen(theme.bg);
  headerTitle = "";
  headerCentre = "";
  headerStatus = HEADER_STATUS_NONE;

  tft.setTextDatum(TL_DATUM);

  uiApplyTypography(TYPE_HEADER, theme.textPrimary);
  tft.setTextColor(theme.textPrimary, theme.bg);
  tft.setCursor(30, 20);
  tft.println("System Settings");

  uiApplyTypography(TYPE_BODY, theme.textSecondary);
  tft.setTextColor(theme.textPrimary, theme.bg);
  tft.setCursor(55, 60);
  tft.println("v1.0.0 - Internet Radio");

    uiApplyTypography(TYPE_BODY, theme.textSecondary);

    int labelX = 98;
    int valueX = 270;
    int y = 98;
    int rowH = 35;

    // Outline Box
    const int panelX = 30;
    const int panelY = 91;
    const int panelW = 425;
    const int panelH = 147;

    tft.drawRoundRect(panelX,     panelY,     panelW,     panelH,     12, tft.color565(0, 90, 220));
    tft.drawRoundRect(panelX + 1, panelY + 1, panelW - 2, panelH - 2, 11, tft.color565(0, 90, 220));

    // WiFi
    drawIcon(WiFi.status() == WL_CONNECTED ? ICON_WIFI_ON : ICON_WIFI_OFF, 50, y - 3, ICON_SMALL);

    tft.setTextColor(theme.textSecondary, theme.bg);
    tft.setCursor(labelX, y);
    tft.print("WiFi");

    tft.setTextColor(theme.textValue, theme.bg);
    tft.setCursor(valueX, y);
    String displaySSID = WiFi.SSID();

    if (displaySSID.length() > 9) {
      displaySSID = displaySSID.substring(0, 9) + "..";
    }

    tft.print(displaySSID);

    // Catalogue
    y += rowH;

    drawIcon(ICON_CATALOGUE, 50, y - 3, ICON_SMALL);

    tft.setTextColor(theme.textSecondary, theme.bg);
    tft.setCursor(labelX, y);
    tft.print("Catalogue");

    tft.setTextColor(theme.textValue, theme.bg);
    tft.setCursor(valueX, y);
    tft.print(stationCatalogueVersion);

    // Stations
    y += rowH;

    drawIcon(ICON_STATIONS, 50, y - 2, ICON_SMALL);

    tft.setTextColor(theme.textSecondary, theme.bg);
    tft.setCursor(labelX, y);
    tft.print("Stations");

    tft.setTextColor(theme.textValue, theme.bg);
    tft.setCursor(valueX, y);
    if (stationLibraryReady) {
      tft.print(stationLibraryTotal);
      tft.print(" Available");
    } else {
      tft.print("Unavailable");
    }

    // IP Address
    y += rowH;

    drawIcon(WiFi.status() == WL_CONNECTED ? ICON_ROUTER_ON : ICON_ROUTER_OFF, 50, y - 1, ICON_SMALL);

    tft.setTextColor(theme.textSecondary, theme.bg);
    tft.setCursor(labelX, y);
    tft.print("IP Address");

    tft.setTextColor(theme.textValue, theme.bg);
    tft.setCursor(valueX, y);
    tft.print(WiFi.localIP());

    safeTftEnd();

  drawButton(stationsButton, false);
  drawButton(wifiSetupButton, false);
  currentScreen = SCREEN_HOME;
}