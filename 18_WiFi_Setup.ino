// ============================================================
// 18_WiFi_Setup.ino
// Wi-Fi Setup Screen
// ============================================================
//
// Purpose:
//
// Implements the branded Wi-Fi configuration screen.
//
// Responsibilities:
//
//   • Display Wi-Fi setup instructions.
//   • Present the WiFiManager access point details.
//   • Integrate with the WiFiManager configuration process.
//
// Connection management is handled by WiFi_Manager.
//
// ============================================================


namespace {

const int WIFI_SETUP_LOGO_X = 15;
const int WIFI_SETUP_LOGO_Y = 50;

const int WIFI_SETUP_TEXT_X = 250;

}  // namespace


// ============================================================
// Draw Wi-Fi Setup Screen
// ============================================================

void drawWiFiSetupScreen() {
  safeTftStart();

  tft.fillScreen(theme.bg);

  // ----------------------------------------------------------
  // Product artwork
  // ----------------------------------------------------------

  drawSplashLogo(
    "/branding/Internet_Radio_220.png",
    WIFI_SETUP_LOGO_X,
    WIFI_SETUP_LOGO_Y,
    theme.bg
  );

  tft.setTextDatum(TL_DATUM);

  // ----------------------------------------------------------
  // Screen title
  // ----------------------------------------------------------

  uiApplyTypography(
    TYPE_HEADER,
    theme.textMuted
  );

  tft.drawString(
    "Wi-Fi Setup",
    WIFI_SETUP_TEXT_X,
    58
  );

  // ----------------------------------------------------------
  // Connection instruction
  // ----------------------------------------------------------

  uiApplyTypography(
    TYPE_STATUS,
    theme.textSecondary
  );

  tft.drawString(
    "Connect your WiFi",
    WIFI_SETUP_TEXT_X,
    112
  );

  tft.drawString(
    "device to:",
    WIFI_SETUP_TEXT_X,
    138
  );

  // ----------------------------------------------------------
  // Temporary setup network
  // ----------------------------------------------------------

  uiApplyTypography(
    TYPE_STATUS,
    theme.wifiGood
  );

  tft.drawString(
    "InternetRadio-Setup",
    WIFI_SETUP_TEXT_X,
    182
  );

  // ----------------------------------------------------------
  // Final instruction
  // ----------------------------------------------------------

  uiApplyTypography(
    TYPE_STATUS,
    theme.textSecondary
  );

  tft.drawString(
    "Follow the setup",
    WIFI_SETUP_TEXT_X,
    238
  );

  tft.drawString(
    "instructions.",
    WIFI_SETUP_TEXT_X,
    264
  );

  safeTftEnd();
}


// ============================================================
// WiFiManager Configuration Portal Callback
// ============================================================

void wifiConfigModeCallback(WiFiManager* wifiManager) {
  wifiSetupWasShown = true;

  Serial.println("WiFi configuration portal started");

  Serial.print("Setup network: ");
  Serial.println(wifiManager->getConfigPortalSSID());

  drawWiFiSetupScreen();
}