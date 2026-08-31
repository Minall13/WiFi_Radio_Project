// ============================================================
// 07_StationProvider.ino
// Station Provider
// ============================================================
//
// Purpose:
//
// Provides a unified interface for loading and accessing
// station catalogues stored in LittleFS.
//
// Responsibilities:
//
//   • Load catalogue pages from LittleFS.
//   • Provide stations to the user interface.
//   • Track the selected station.
//   • Hide catalogue storage details from the application.
//
// StationProvider is the application's single point of access
// to station data.
//
// ============================================================


// Catalogue metadata used by the Home screen.
String stationCatalogueVersion = "Unknown";
int stationLibraryTotal = 0;
bool stationLibraryReady = false;


// ============================================================
// Provider Initialisation
// ============================================================

void stationProviderBegin() {
  File file = LittleFS.open("/station_library.json", "r");

  if (!file) {
    Serial.println("Station library open failed.");

    stationCatalogueVersion = "Unavailable";
    stationLibraryTotal = 0;
    stationLibraryReady = false;

    return;
  }

  // Retain only the catalogue metadata.
  // This avoids loading the full 657 KB JSON file into RAM.
  JsonDocument filter;
  filter["catalogue_version"] = true;
  filter["station_count"] = true;

  JsonDocument document;

  DeserializationError error = deserializeJson(
    document,
    file,
    DeserializationOption::Filter(filter)
  );

  file.close();

  if (error) {
    Serial.print("Station library validation failed: ");
    Serial.println(error.c_str());

    stationCatalogueVersion = "Invalid";
    stationLibraryTotal = 0;
    stationLibraryReady = false;

    return;
  }

  stationCatalogueVersion =
    document["catalogue_version"] | "Unknown";

  stationLibraryTotal =
    document["station_count"] | 0;

  stationLibraryReady =
    stationLibraryTotal > 0;

  Serial.println();
  Serial.println("Station Library");
  Serial.println("---------------");

  Serial.print("Catalogue version: ");
  Serial.println(stationCatalogueVersion);

  Serial.print("Station count:     ");
  Serial.println(stationLibraryTotal);

  Serial.println();
}


// ============================================================
// Station Loading Router
// ============================================================

bool stationProviderLoad(const String& filterName) {
  stationCount = 0;
  stationPageStart = 0;
  selectedStationIndex = -1;

  return loadLittleFSStations(filterName);
}


// ============================================================
// LittleFS Station Loader
// ============================================================

bool loadLittleFSStations(const String& filterName) {
  String fileFilter = filterName;
  fileFilter.toLowerCase();

  char path[48];

  snprintf(
    path,
    sizeof(path),
    "/catalogues/%s_%03d.json",
    fileFilter.c_str(),
    0
  );

  Serial.print("Loading station catalogue: ");
  Serial.println(path);

  File file = LittleFS.open(path, "r");

  if (!file) {
    Serial.println("Catalogue page open failed.");
    return false;
  }

  JsonDocument document;

  DeserializationError error =
    deserializeJson(document, file);

  file.close();

  if (error) {
    Serial.print("Catalogue page parse failed: ");
    Serial.println(error.c_str());
    return false;
  }

  stationCount = 0;

  JsonArray records = document["stations"].as<JsonArray>();

  for (JsonObject record : records) {
    if (stationCount >= MAX_STATIONS) {
      break;
    }

    stations[stationCount].name =
      String(record["name"] | "");

    stations[stationCount].streamUrl =
      String(record["url"] | "");

    stations[stationCount].resolvedUrl =
      stations[stationCount].streamUrl;

    stations[stationCount].homepage =
      String(record["homepage"] | "");

    stations[stationCount].metadataService =
      String(record["metadata_service"] | "");

    stations[stationCount].favicon =
      String(record["favicon"] | "");

    stations[stationCount].logo =
      String(record["logo"] | "/logos/default_station.raw565");

    stations[stationCount].tags =
      String(record["tags"] | "");

    stations[stationCount].codec =
      String(record["codec"] | "");

    stations[stationCount].bitrate =
      record["bitrate"] | 0;

    stations[stationCount].isHLS =
      stations[stationCount].streamUrl.indexOf(".m3u8") >= 0;

    stationCount++;
  }

  Serial.print("Catalogue stations loaded: ");
  Serial.println(stationCount);

  return stationCount > 0;
}