// ============================================================
// 04_WiFi_Manager.ino
// Wi-Fi Connection Manager
// ============================================================
//
// Purpose:
//
// Establishes a Wi-Fi connection using stored credentials or,
// if necessary, launches the WiFiManager configuration portal.
//
// Responsibilities:
//
//   • Connect using saved Wi-Fi credentials.
//   • Launch the branded setup screen when provisioning.
//   • Restore the startup sequence after provisioning.
//   • Reset stored Wi-Fi credentials.
//
// Network configuration is intentionally separated from the
// user interface. Screen drawing is handled by
// 17_Splash.ino and 19_WiFi_Setup.ino.
//
// ============================================================

bool wifiSetupWasShown = false;

void connectWiFi() {
  WiFiManager wm;

  wm.setAPCallback(wifiConfigModeCallback);

  bool connected = wm.autoConnect(
    "InternetRadio-Setup"
  );

  if (!connected) {
    splashStatus("Wi-Fi connection failed");

    Serial.println("WiFi connection failed");

    delay(2000);
    ESP.restart();
  }

  Serial.print("WiFi connected. SSID: ");
  Serial.println(WiFi.SSID());

  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  // Restore the startup splash only when the setup portal
  // has actually been displayed.
  if (wifiSetupWasShown) {
    splashBegin();
    splashStatus("Wi-Fi connected");
    delay(1000);
  }
}

void resetWiFiSettings() {
  Serial.println("Resetting WiFi settings...");

  showMessage("Reset WiFi...");

  WiFiManager wm;
  wm.resetSettings();

  delay(1500);

  showMessage("Restarting...");
  delay(1500);

  ESP.restart();
}