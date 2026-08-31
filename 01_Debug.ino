// ============================================================
// Debug
// ============================================================
//
// Temporary debug helpers.
//
// Enable individual debug flags as required during development.
//
// ============================================================

const bool DEBUG_PRINT_STATION_METADATA = false;

const bool DEBUG_WIFI = false;
const bool DEBUG_TOUCH = false;
const bool DEBUG_RADIO_BROWSER = false;
const bool DEBUG_PLAYER = false;

void printStationMetadata(const Station& station) {
  if (!DEBUG_PRINT_STATION_METADATA) return;

  Serial.println();
  Serial.println("========================================");
  Serial.println(station.name);

  Serial.print("URL         : ");
  Serial.println(station.streamUrl);

  Serial.print("Resolved    : ");
  Serial.println(station.resolvedUrl);

  Serial.print("Codec       : ");
  Serial.println(station.codec);

  Serial.print("Bitrate     : ");
  Serial.println(station.bitrate);

  Serial.print("Tags        : ");
  Serial.println(station.tags);

  Serial.print("Homepage    : ");
  Serial.println(station.homepage);

  Serial.print("Favicon     : ");
  Serial.println(station.favicon);

  Serial.print("HLS         : ");
  Serial.println(station.isHLS ? "Yes" : "No");

  Serial.println("========================================");
}