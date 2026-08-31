// ============================================================
// Internet Radio Project
// Main application entry point
// ============================================================
//
// Responsibilities:
//
//   • Include required libraries
//   • Define shared structures and global state
//   • Initialise display, touch, LittleFS, WiFi and typography
//   • Start the initial UI screen
//
// ============================================================

#include <WiFiManager.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <PNGdec.h>

// ============================================================
// Display Hardware
// ============================================================

SPIClass touchSPI(HSPI);

#define TFT_CS_PIN 10
#define TOUCH_CS   14

#define TOUCH_MIN_X 300
#define TOUCH_MAX_X 3820
#define TOUCH_MIN_Y 420
#define TOUCH_MAX_Y 3770

TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen ts(TOUCH_CS);

// ============================================================
// Audio Hardware
// ============================================================

#define I2S_BCLK  6
#define I2S_LRCK  7
#define I2S_DOUT  5

// ============================================================
// Shared UI Structures
// ============================================================

struct Button {
  int x;
  int y;
  int w;
  int h;
  const char* label;
};

enum PlayerSymbol {
  PLAYER_SYMBOL_BACK,
  PLAYER_SYMBOL_STOP,
  PLAYER_SYMBOL_DOWN,
  PLAYER_SYMBOL_UP
};

struct UITheme {
  uint16_t bg;
  uint16_t panel;
  uint16_t headerBg;
  uint16_t footerBg;

  uint16_t textPrimary;
  uint16_t textSecondary;
  uint16_t textMuted;
  uint16_t textValue;
  uint16_t textStrong;

  uint16_t accent;
  uint16_t buttonBg;
  uint16_t playerButtonBg;
  uint16_t buttonBorder;
  uint16_t buttonText;

  uint16_t wifiGood;
  uint16_t wifiWeak;
  uint16_t wifiOff;

  uint16_t playerBack;
  uint16_t playerStop;
  uint16_t playerVolumeDown;
  uint16_t playerMute;
  uint16_t playerVolumeUp;
};

extern UITheme theme;

// ============================================================
// Theme IDs
// ============================================================

enum ThemeID {
  THEME_MIDNIGHT,
  THEME_GRAPHITE,
  THEME_OCEAN,
  THEME_AMBER,
  THEME_RETRO_GREEN,
  THEME_DAYLIGHT,
  THEME_COUNT
};

const char* const themeNames[THEME_COUNT] = {
  "Midnight",
  "Graphite",
  "Ocean",
  "Amber",
  "Retro Green",
  "Daylight"
};

// ============================================================
// Screen / Typography IDs
// ============================================================

enum ScreenMode {
  SCREEN_HOME,
  SCREEN_STATIONS,
  SCREEN_FILTERS,
  SCREEN_PLAYER
};

enum UITypeStyle {
  TYPE_DISPLAY,
  TYPE_HEADER,
  TYPE_SECTION,
  TYPE_BODY,
  TYPE_BUTTON,
  TYPE_CAPTION,
  TYPE_STATUS
};

// ============================================================
// Icon IDs
// ============================================================

enum IconId {
  ICON_WIFI_ON,
  ICON_WIFI_OFF,

  ICON_CATALOGUE,
  ICON_STATIONS,

  ICON_ROUTER_ON,
  ICON_ROUTER_OFF,

  ICON_BACK,
  ICON_SETTINGS
};

enum IconSize {
  ICON_SMALL,   // 24
  ICON_MEDIUM,  // 32
  ICON_LARGE    // 48
};

// ============================================================
// Runtime UI State
// ============================================================

ScreenMode currentScreen = SCREEN_HOME;

String headerTitle  = "";
String headerCentre = "";
uint8_t headerStatus = 0;

// ============================================================
// Header Status Icons
// ============================================================

const uint8_t HEADER_STATUS_NONE = 0;
const uint8_t HEADER_STATUS_WIFI = 1;

// ============================================================
// Buttons
// ============================================================

Button backFooterButton     = {10, 255, 45, 45, ""};

Button stationSelectButton1 = {68, 255, 60, 45, "1"};
Button stationSelectButton2 = {138, 255, 60, 45, "2"};
Button stationSelectButton3 = {208, 255, 60, 45, "3"};
Button stationSelectButton4 = {278, 255, 60, 45, "4"};
Button stationSelectButton5 = {348, 255, 60, 45, "5"};

Button settingsFooterButton = {425, 255, 45, 45, ""};

Button stationsButton       = {25, 245, 200, 55, "Filter"};
Button wifiSetupButton      = {255, 245, 200, 55, "WiFi"};

Button filterMenuButton     = {25, 10, 180, 52, ""};

Button filterBBCButton      = {25, 55, 135, 60, "BBC"};
Button filterAbsoluteButton = {172, 55, 135, 60, "Absolute"};
Button filterVirginButton   = {319, 55, 135, 60, "Virgin"};

Button filterCapitalButton  = {25, 125, 135, 60, "Capital"};
Button filterHeartButton    = {172, 125, 135, 60, "Heart"};
Button filterKissButton     = {319, 125, 135, 60, "Kiss"};

Button filterRockButton     = {25, 193, 135, 60, "Rock"};
Button filterTalkButton     = {172, 193, 135, 60, "Talk"};
Button filterAllButton      = {319, 193, 135, 60, "All"};

Button filterSettingsButton = {110, 266, 260, 40, "Settings"};

Button stationPrevButton    = {425, 70, 45, 73, "^"};
Button stationNextButton    = {425, 152, 45, 73, "v"};

// ============================================================
// Player Footer Buttons
// ============================================================

Button playerBackButton       = { 10, 265, 88, 45, "" };
Button playerStopButton       = {103, 265, 88, 45, ""};
Button playerVolumeDownButton = {196, 265, 88, 45, ""};
Button playerMuteButton       = {289, 265, 88, 45, ""};
Button playerVolumeUpButton   = {382, 265, 88, 45, ""};

// ============================================================
// Message State
// ============================================================

String pendingMessage = "";
bool messagePending = false;

// ============================================================
// Station Data
// ============================================================

const int MAX_STATIONS = 80;
const int STATIONS_PER_PAGE = 5;

struct Station {
  String name;
  String streamUrl;
  String resolvedUrl;
  String homepage;
  String metadataService;
  String favicon;
  String logo;
  String tags;
  String codec;
  int bitrate;
  bool isHLS;
};

Station stations[MAX_STATIONS];
int stationCount = 0;
int stationPageStart = 0;
int selectedStationIndex = -1;

// ============================================================
// Station Filters
// ============================================================

const char* stationFilters[] = {
  "BBC",
  "Absolute",
  "Virgin",
  "Capital",
  "Heart",
  "Kiss",
  "Rock",
  "Talk",
  "All"
};

const int stationFilterCount = 9;
int currentStationFilter = 0;

// ============================================================
// Radio Browser
// ============================================================

const char* radioBrowserServers[] = {
  "https://de1.api.radio-browser.info",
  "https://de2.api.radio-browser.info",
  "https://nl1.api.radio-browser.info"
};

const int radioBrowserServerCount = 3;
const int RADIO_HTTP_TIMEOUT_MS = 2500;

// ============================================================
// Screen Layout
// ============================================================

const int HEADER_Y = 0;
const int HEADER_H = 50;

const int CONTENT_Y = 50;
const int CONTENT_H = 190;

const int FOOTER_Y = 240;
const int FOOTER_H = 80;

// ============================================================
// Developer Options
// ============================================================

const bool DEV_FONT_PREVIEW = false;

// ============================================================
// WiFi Reset State
// ============================================================

bool confirmWiFiReset = false;
unsigned long confirmWiFiResetStart = 0;

// ============================================================
// Function Prototypes
// ============================================================

// ---------- Initialisation ----------
void connectWiFi();
void testRadioBrowser();

// ---------- Audio ----------
extern String currentStreamTitle;
extern bool streamTitleChanged;

extern String currentBBCProgramme;
extern String currentBBCEndTime;
extern bool bbcMetadataChanged;

bool fetchBBCMetadata(const String& service);
void bbcMetadataBegin(const String& service);
void bbcMetadataStop();
void bbcMetadataLoop();

bool audioBegin();
void audioLoop();
bool audioPlay(const String& url);
void audioStop();

// ---------- Splash ----------
void splashBegin();
void splashStatus(const String& message);
void splashReady();

// ---------- Screen Rendering ----------
void drawHomeScreen();
void drawFilterScreen();
void drawStationsScreen();
void drawPlayerScreen();
void drawFontPreviewScreen();

// ---------- Typography ----------
void uiInitTypography();
void uiApplyTypography(UITypeStyle style, uint16_t colour);

// ============================================================
// Setup
// ============================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Internet Radio");

  pinMode(TFT_CS_PIN, OUTPUT);
  pinMode(TOUCH_CS, OUTPUT);

  digitalWrite(TFT_CS_PIN, HIGH);
  digitalWrite(TOUCH_CS, HIGH);

  tft.init();
  tft.setRotation(1);

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
  } else {
    Serial.println("LittleFS mounted.");
    stationProviderBegin();
  }

  uiInitTypography();

  if (DEV_FONT_PREVIEW) {
    drawFontPreviewScreen();
    return;
  }

  splashBegin();

  splashStatus("Connecting to Wi-Fi...");
  connectWiFi();

  splashStatus("Initialising audio...");
  audioBegin();

  stationProviderLoad("All");

  ts.begin(touchSPI);
  ts.setRotation(1);

  splashReady();

  Serial.println("Setup complete");
}