// ============================================================
// 05_RadioBrowser.ino
// Radio Browser Provider
// ============================================================
//
// Purpose:
//
// Implements the Radio Browser station provider used by the StationProvider
// abstraction.
//
// Responsibilities:
//
//   • Retrieve station information from Radio Browser.
//   • Support catalogue creation and maintenance.
//   • Remain independent of the user interface and audio engine.
//
// The application accesses stations through the StationProvider
// abstraction. This module implements the Radio Browser
// provider only.
//
// ============================================================


void testRadioBrowser() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Radio Browser test skipped: WiFi not connected");
    return;
  }

  Serial.println("Radio Browser test starting...");

  stationCount = 0;

  for (int serverIndex = 0; serverIndex < radioBrowserServerCount; serverIndex++) {
    String filter = stationFilters[currentStationFilter];

    String apiUrl;

    if (filter == "All") {
      apiUrl = String(radioBrowserServers[serverIndex]) +
              "/json/stations/search?countrycode=GB&limit=25&hidebroken=true&order=votes&reverse=true";
    }
    else {
      apiUrl = String(radioBrowserServers[serverIndex]) +
              "/json/stations/search?countrycode=GB&limit=25&hidebroken=true&name=" +
              filter;
    }

    Serial.print("Trying Radio Browser server: ");
    Serial.println(radioBrowserServers[serverIndex]);

    HTTPClient http;
    http.begin(apiUrl);
    http.setTimeout(RADIO_HTTP_TIMEOUT_MS);

    int httpCode = http.GET();

    Serial.print("HTTP response code: ");
    Serial.println(httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();

      Serial.print("Payload length: ");
      Serial.println(payload.length());

      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("JSON parse failed: ");
        Serial.println(error.c_str());
        http.end();
        continue;
      }

      Serial.println("Stations found:");

      for (JsonObject station : doc.as<JsonArray>()) {
        const char* name = station["name"];
        const char* url = station["url"];
        const char* urlResolved = station["url_resolved"];
        const char* homepage = station["homepage"];
        const char* favicon = station["favicon"];
        const char* tags = station["tags"];
        const char* codec = station["codec"];
        int bitrate = station["bitrate"] | 0;

        String stationName = String(name);
        stationName.trim();
        stationName.replace(" (128k)", "");
        stationName.replace(" [MP3]", "");

        if (stationName.indexOf("TV") >= 0 ||
            stationName.indexOf("tv") >= 0 ||
            stationName.indexOf("News HD") >= 0) {
          Serial.print("Skipped: ");
          Serial.println(stationName);
          continue;
        }

        bool duplicate = false;

        for (int i = 0; i < stationCount; i++) {
          if (stations[i].name == stationName) {
            duplicate = true;
            break;
          }
        }

        if (!duplicate && stationCount < MAX_STATIONS) {
          stations[stationCount].name = stationName;
          stations[stationCount].streamUrl = String(url);
          stations[stationCount].resolvedUrl = String(urlResolved);
          stations[stationCount].homepage = String(homepage);
          stations[stationCount].favicon = String(favicon);
          stations[stationCount].tags = String(tags);
          stations[stationCount].codec = String(codec);
          stations[stationCount].bitrate = bitrate;
          stations[stationCount].isHLS = stations[stationCount].streamUrl.indexOf(".m3u8") >= 0 ||
                                        stations[stationCount].resolvedUrl.indexOf(".m3u8") >= 0;
          printStationMetadata(stations[stationCount]);

          stationCount++;
        }

      }

      Serial.print("Stations stored: ");
      Serial.println(stationCount);

      http.end();

      if (stationCount > 0) {
        Serial.print("Radio Browser server used: ");
        Serial.println(radioBrowserServers[serverIndex]);
        return;
      }
    }
    else {
      Serial.println("HTTP request failed, trying next server...");
    }

    http.end();
  }

  Serial.println("All Radio Browser servers failed");
  showMessage("Radio API failed");
}