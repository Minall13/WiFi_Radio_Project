// ============================================================
// 03_WiFi_Manager.ino
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

// ============================================================
// Stored SSIDs
// ============================================================

Preferences wifiPrefs;

const int WIFI_MAX_SAVED_NETWORKS = 3;
const unsigned long WIFI_CONNECT_TIMEOUT_MS = 10000;

struct SavedWiFiNetwork {
  String ssid;
  String password;
};

SavedWiFiNetwork savedNetworks[WIFI_MAX_SAVED_NETWORKS];

void loadSavedWiFiNetworks() {
  wifiPrefs.begin("wifi-list", true);

  for (int i = 0; i < WIFI_MAX_SAVED_NETWORKS; i++) {
    String ssidKey = "ssid" + String(i);
    String passKey = "pass" + String(i);

    savedNetworks[i].ssid = wifiPrefs.getString(
      ssidKey.c_str(),
      ""
    );

    savedNetworks[i].password = wifiPrefs.getString(
      passKey.c_str(),
      ""
    );
  }

  Serial.println("Saved Wi-Fi order:");

  for (int i = 0; i < WIFI_MAX_SAVED_NETWORKS; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(savedNetworks[i].ssid);
  }

  wifiPrefs.end();
}

void saveSavedWiFiNetworks() {
  wifiPrefs.begin("wifi-list", false);

  for (int i = 0; i < WIFI_MAX_SAVED_NETWORKS; i++) {
    String ssidKey = "ssid" + String(i);
    String passKey = "pass" + String(i);

    wifiPrefs.putString(
      ssidKey.c_str(),
      savedNetworks[i].ssid
    );

    wifiPrefs.putString(
      passKey.c_str(),
      savedNetworks[i].password
    );
  }

  wifiPrefs.end();
}

void rememberWiFiNetwork(
  const String& ssid,
  const String& password
) {
  if (ssid.length() == 0) {
    return;
  }

  int existingIndex = -1;

  for (int i = 0; i < WIFI_MAX_SAVED_NETWORKS; i++) {
    if (savedNetworks[i].ssid == ssid) {
      existingIndex = i;
      break;
    }
  }

  if (existingIndex == 0) {
    savedNetworks[0].password = password;
    saveSavedWiFiNetworks();
    return;
  }

  SavedWiFiNetwork selectedNetwork;
  selectedNetwork.ssid = ssid;
  selectedNetwork.password = password;

  if (existingIndex > 0) {
    for (int i = existingIndex; i > 0; i--) {
      savedNetworks[i] = savedNetworks[i - 1];
    }
  } else {
    for (int i = WIFI_MAX_SAVED_NETWORKS - 1; i > 0; i--) {
      savedNetworks[i] = savedNetworks[i - 1];
    }
  }

  savedNetworks[0] = selectedNetwork;

  saveSavedWiFiNetworks();
}

bool connectToSavedWiFi() {
  loadSavedWiFiNetworks();

  WiFi.mode(WIFI_STA);

  int networkCount = WiFi.scanNetworks();

  bool attempted[WIFI_MAX_SAVED_NETWORKS] = { false };

  // First pass:
  // Try saved networks that were seen in the scan,
  // in most-recently-used order.
  if (networkCount > 0) {
    for (int i = 0; i < WIFI_MAX_SAVED_NETWORKS; i++) {
      if (savedNetworks[i].ssid.length() == 0) {
        continue;
      }

      bool networkVisible = false;

      for (int n = 0; n < networkCount; n++) {
        if (WiFi.SSID(n) == savedNetworks[i].ssid) {
          networkVisible = true;
          break;
        }
      }

      if (!networkVisible) {
        continue;
      }

      attempted[i] = true;

      WiFi.begin(
        savedNetworks[i].ssid.c_str(),
        savedNetworks[i].password.c_str()
      );

      unsigned long startTime = millis();

      while (
        WiFi.status() != WL_CONNECTED &&
        millis() - startTime < WIFI_CONNECT_TIMEOUT_MS
      ) {
        delay(250);
      }

      if (WiFi.status() == WL_CONNECTED) {
        WiFi.scanDelete();

        rememberWiFiNetwork(
          savedNetworks[i].ssid,
          savedNetworks[i].password
        );

        return true;
      }

      WiFi.disconnect();
      delay(250);
    }
  }

  WiFi.scanDelete();

  // Second pass:
  // Try any saved networks that were not seen in the scan.
  // A scan result is useful guidance, but not proof that an
  // access point is unavailable.
  for (int i = 0; i < WIFI_MAX_SAVED_NETWORKS; i++) {
    if (
      savedNetworks[i].ssid.length() == 0 ||
      attempted[i]
    ) {
      continue;
    }

    WiFi.begin(
      savedNetworks[i].ssid.c_str(),
      savedNetworks[i].password.c_str()
    );

    unsigned long startTime = millis();

    while (
      WiFi.status() != WL_CONNECTED &&
      millis() - startTime < WIFI_CONNECT_TIMEOUT_MS
    ) {
      delay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
      rememberWiFiNetwork(
        savedNetworks[i].ssid,
        savedNetworks[i].password
      );

      return true;
    }

    WiFi.disconnect();
    delay(250);
  }

  return false;
}

// ============================================================
// Wi-Fi Startup
// ============================================================

bool wifiSetupWasShown = false;

void connectWiFi() {

  Serial.println();
  Serial.println("HEAP BEFORE WIFI:");
  Serial.printf(
    "Internal free: %u\n",
    heap_caps_get_free_size(MALLOC_CAP_INTERNAL)
  );
  Serial.printf(
    "Largest block: %u\n",
    heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL)
  );

  // First try any of our three remembered networks that are
  // actually visible at the current location.

  if (connectToSavedWiFi()) {
    Serial.println();
    Serial.println("HEAP AFTER SAVED WIFI CONNECT:");
    Serial.printf(
      "Internal free: %u\n",
      heap_caps_get_free_size(MALLOC_CAP_INTERNAL)
    );
    Serial.printf(
      "Largest block: %u\n",
      heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL)
    );

    Serial.print("WiFi connected. SSID: ");
    Serial.println(WiFi.SSID());

    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());

    return;
  }

  // No remembered network was available or could connect.
  // Fall back to the existing WiFiManager provisioning flow.

  WiFiManager wm;

  wm.setAPCallback(wifiConfigModeCallback);

  // Prevent WiFiManager spending several minutes trying an
  // unavailable previously stored network.

  wm.setConnectTimeout(10);

  bool connected = wm.autoConnect(
    "InternetRadio-Setup"
  );

  if (!connected) {
    splashStatus("Wi-Fi connection failed");

    Serial.println("WiFi connection failed");

    delay(2000);
    ESP.restart();
  }

  String connectedSSID = WiFi.SSID();
  String connectedPassword = WiFi.psk();

  rememberWiFiNetwork(
    connectedSSID,
    connectedPassword
  );

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

  wifiPrefs.begin("wifi-list", false);
  wifiPrefs.clear();
  wifiPrefs.end();

  for (int i = 0; i < WIFI_MAX_SAVED_NETWORKS; i++) {
    savedNetworks[i].ssid = "";
    savedNetworks[i].password = "";
  }

  delay(1500);

  showMessage("Restarting...");
  delay(1500);

  ESP.restart();
}