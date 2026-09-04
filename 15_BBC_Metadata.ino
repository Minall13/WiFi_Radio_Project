// ============================================================
// 15_BBC_Metadata.ino
// BBC Programme Metadata
// ============================================================
//
// Purpose:
//
// Retrieves and manages programme metadata for BBC stations.
//
// Responsibilities:
//
//   • Request BBC programme information.
//   • Parse programme title and timing data.
//   • Maintain the current programme information.
//   • Signal when displayed metadata has changed.
//
// This module is independent of station playback and screen
// rendering.
//
// ============================================================


// ============================================================
// Configuration
// ============================================================

const unsigned long BBC_METADATA_INITIAL_DELAY_MS = 2000;
const unsigned long BBC_METADATA_REFRESH_MS       = 5UL * 60UL * 1000UL;
const unsigned long BBC_METADATA_RETRY_MS         = 30UL * 1000UL;


// ============================================================
// Current BBC Programme
// ============================================================

// These values belong only to the BBC station currently playing.

String currentBBCService   = "";
String currentBBCProgramme = "";
String currentBBCEndTime   = "";

bool bbcMetadataChanged = false;


// ============================================================
// Request State
// ============================================================

bool bbcMetadataActive  = false;
bool bbcMetadataPending = false;

unsigned long bbcMetadataNextRequestAt = 0;


// ============================================================
// Timing Helper
// ============================================================

bool bbcMetadataTimeReached(unsigned long targetTime) {
  return static_cast<long>(millis() - targetTime) >= 0;
}


// ============================================================
// Reset Current Metadata
// ============================================================

void clearBBCMetadata() {
  currentBBCProgramme = "";
  currentBBCEndTime   = "";

  bbcMetadataChanged = true;
}


// ============================================================
// BBC RMS Request
// ============================================================

bool fetchBBCMetadata(const String& service) {
  if (service.length() == 0) {
    return false;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("BBC metadata unavailable: ");
    Serial.println(service);
    return false;
  }

  const char* host = "rms.api.bbc.co.uk";

  String path =
    "/v2/broadcasts/poll/" +
    service +
    "?limit=1";

  Serial.println();
  Serial.print("BBC metadata request: ");
  Serial.println(service);

  Serial.print("Free heap before BBC metadata: ");
  Serial.println(ESP.getFreeHeap());

  Serial.print("Internal heap before BBC metadata: ");
  Serial.println(
    heap_caps_get_free_size(
      MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    )
  );

  WiFiClientSecure secureClient;

  secureClient.setInsecure();
  secureClient.setHandshakeTimeout(15);

  unsigned long requestStartedAt = millis();

  if (!secureClient.connect(host, 443)) {
    Serial.print("BBC metadata connection failed: ");
    Serial.println(service);

    secureClient.stop();
    return false;
  }

  secureClient.print(
    String("GET ") + path + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "User-Agent: InternetRadioProject/0.9\r\n" +
    "Accept: application/json\r\n" +
    "Connection: close\r\n\r\n"
  );

  unsigned long responseTimeoutStartedAt = millis();

  while (
    !secureClient.available() &&
    secureClient.connected() &&
    millis() - responseTimeoutStartedAt < 15000
  ) {
    audioLoop();
    delay(10);
  }

  if (!secureClient.available()) {
    Serial.print("BBC metadata response timed out: ");
    Serial.println(service);

    secureClient.stop();
    return false;
  }

  String statusLine = secureClient.readStringUntil('\n');
  statusLine.trim();

  if (!statusLine.startsWith("HTTP/1.1 200")) {
    Serial.print("BBC metadata request failed: ");
    Serial.print(service);
    Serial.print(" — ");
    Serial.println(statusLine);

    secureClient.stop();
    return false;
  }

  while (secureClient.connected()) {
    String headerLine = secureClient.readStringUntil('\n');

    if (headerLine == "\r") {
      break;
    }

    audioLoop();
  }

  JsonDocument filter;

  filter["data"][0]["titles"]["primary"] = true;
  filter["data"][0]["end"]               = true;

  JsonDocument document;

  DeserializationError error =
    deserializeJson(
      document,
      secureClient,
      DeserializationOption::Filter(filter)
    );

  secureClient.stop();

  if (error) {
    Serial.print("BBC metadata parse failed: ");
    Serial.print(service);
    Serial.print(" — ");
    Serial.println(error.c_str());

    return false;
  }

  String programme =
    String(
      document["data"][0]["titles"]["primary"] | ""
    );

  String endTime =
    String(
      document["data"][0]["end"] | ""
    );

  programme.trim();
  endTime.trim();

  if (programme.length() == 0) {
    Serial.print("BBC programme unavailable: ");
    Serial.println(service);
    return false;
  }

  bool changed =
    programme != currentBBCProgramme ||
    endTime   != currentBBCEndTime;

  currentBBCProgramme = programme;
  currentBBCEndTime   = endTime;

  if (changed) {
    bbcMetadataChanged = true;
  }

  Serial.print("BBC programme: ");
  Serial.println(currentBBCProgramme);

  Serial.print("BBC programme end: ");
  Serial.println(currentBBCEndTime);

  Serial.print("BBC metadata duration: ");
  Serial.print(millis() - requestStartedAt);
  Serial.println(" ms");

  Serial.print("Internal heap after BBC metadata: ");
  Serial.println(
    heap_caps_get_free_size(
      MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    )
  );

  return true;
}


// ============================================================
// Begin Metadata for Selected BBC Station
// ============================================================

void bbcMetadataBegin(const String& service) {
  currentBBCService = service;

  clearBBCMetadata();

  if (currentBBCService.length() == 0) {
    bbcMetadataActive  = false;
    bbcMetadataPending = false;
    return;
  }

  Serial.print("BBC metadata scheduled: ");
  Serial.println(currentBBCService);

  bbcMetadataActive  = true;
  bbcMetadataPending = true;

  bbcMetadataNextRequestAt =
    millis() + BBC_METADATA_INITIAL_DELAY_MS;
}


// ============================================================
// Stop BBC Metadata
// ============================================================

void bbcMetadataStop() {
  currentBBCService = "";

  bbcMetadataActive  = false;
  bbcMetadataPending = false;

  clearBBCMetadata();
}


// ============================================================
// BBC Metadata Loop
// ============================================================

void bbcMetadataLoop() {
  if (!bbcMetadataActive) {
    return;
  }

  if (!bbcMetadataPending) {
    return;
  }

  if (!bbcMetadataTimeReached(bbcMetadataNextRequestAt)) {
    return;
  }

  bbcMetadataPending = false;

  bool success =
    fetchBBCMetadata(currentBBCService);

  bbcMetadataPending = true;

  if (success) {
    bbcMetadataNextRequestAt =
      millis() + BBC_METADATA_REFRESH_MS;
  } else {
    bbcMetadataNextRequestAt =
      millis() + BBC_METADATA_RETRY_MS;
  }
}