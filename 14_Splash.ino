// ============================================================
// 14_Splash.ino
// Startup Splash Screen
// ============================================================
//
// Purpose:
//
// Implements the branded startup splash screen.
//
// Responsibilities:
//
//   • Display application branding.
//   • Present startup progress messages.
//   • Indicate when startup is complete.
//
// The splash screen is displayed only during application
// startup and hands control to the main application once
// initialisation is complete.
//
// ============================================================


// ============================================================
// Splash Drawing State
// ============================================================
//
// PNGdec draws one scan line at a time through a callback.
//
// These values retain the splash destination and background while
// the PNG is being decoded synchronously.
// ============================================================

static int splashLogoX = 0;
static int splashLogoY = 0;
static uint32_t splashLogoBackground = 0;


// Dedicated scan-line buffer for the 220-pixel splash artwork.
//
// This does not replace or alter the 190-pixel station-artwork
// buffer in 16_Station_Artwork.ino.
static uint16_t splashLineBuffer[220];


namespace {

const char* SPLASH_LOGO_PATH =
  "/branding/Internet_Radio_220.png";

const int SPLASH_LOGO_SIZE = 220;

const int SPLASH_LOGO_X =
  (480 - SPLASH_LOGO_SIZE) / 2;

const int SPLASH_LOGO_Y = 24;


// Only this lower region is refreshed as startup progresses,
// leaving the branded artwork undisturbed.
const int SPLASH_STATUS_Y = 258;
const int SPLASH_STATUS_H = 34;

}


// ============================================================
// Draw Splash Scan Line
// ============================================================
//
// PNGdec calls this once for each decoded image row.
//
// The shared stationPng decoder converts the row to RGB565, but
// writes it into the dedicated 220-pixel splash buffer.
//
// No station-artwork geometry or buffer is used here.
// ============================================================

int splashLogoDraw(PNGDRAW* draw) {
  stationPng.getLineAsRGB565(
    draw,
    splashLineBuffer,
    PNG_RGB565_BIG_ENDIAN,
    splashLogoBackground
  );

  tft.pushImage(
    splashLogoX,
    splashLogoY + draw->y,
    draw->iWidth,
    1,
    splashLineBuffer
  );

  return 1;
}


// ============================================================
// Draw Splash Logo
// ============================================================
//
// Opens and decodes the branded PNG from LittleFS.
//
// The existing PNG decoder and LittleFS callbacks from
// 16_Station_Artwork.ino are reused because the splash is drawn
// before any Player station artwork is required.
//
// The dedicated splash callback ensures that the 220-pixel logo
// never uses the station renderer's 190-pixel scan-line buffer.
// ============================================================

bool drawSplashLogo(
  const char* path,
  int x,
  int y,
  uint16_t backgroundColour
) {
  splashLogoX = x;
  splashLogoY = y;

  splashLogoBackground =
    rgb565ToRgb888(backgroundColour);

  int result = stationPng.open(
    path,
    stationArtworkOpen,
    stationArtworkClose,
    stationArtworkRead,
    stationArtworkSeek,
    splashLogoDraw
  );

  if (result != PNG_SUCCESS) {
    Serial.print("Splash logo open failed: ");
    Serial.print(path);
    Serial.print(" (");
    Serial.print(result);
    Serial.println(")");

    return false;
  }

  result = stationPng.decode(nullptr, 0);
  stationPng.close();

  if (result != PNG_SUCCESS) {
    Serial.print("Splash logo decode failed: ");
    Serial.print(path);
    Serial.print(" (");
    Serial.print(result);
    Serial.println(")");

    return false;
  }

  return true;
}


// ============================================================
// Begin Splash
// ============================================================
//
// Draws the static branded area and establishes the initial
// startup status.
//
// No startup work is performed here. setup() remains responsible
// for Wi-Fi, BBC metadata and audio initialisation in the required
// memory-safe order.
// ============================================================

void splashBegin() {
  tft.fillScreen(theme.bg);

  drawSplashLogo(
    SPLASH_LOGO_PATH,
    SPLASH_LOGO_X,
    SPLASH_LOGO_Y,
    theme.bg
  );

  splashStatus("Starting...");
}


// ============================================================
// Update Splash Status
// ============================================================
//
// Repaints only the status strip beneath the logo.
//
// This avoids unnecessary full-screen redraws while setup()
// progresses through each startup stage.
// ============================================================

void splashStatus(const String& message) {
  tft.fillRect(
    0,
    SPLASH_STATUS_Y,
    480,
    SPLASH_STATUS_H,
    theme.bg
  );

  uiApplyTypography(
    TYPE_STATUS,
    theme.textSecondary
  );

  tft.setTextDatum(MC_DATUM);

  tft.drawString(
    message,
    240,
    SPLASH_STATUS_Y + 16
  );
}


// ============================================================
// Show Ready State
// ============================================================
//
// Preserves the final startup status briefly, confirms that the
// application is ready, then displays the touch prompt.
//
// Touch handling remains elsewhere so this module stays
// presentation-only.
// ============================================================

void splashReady() {

  // Leave the final startup status visible briefly.
  delay(2000);

  tft.fillRect(
    0,
    SPLASH_STATUS_Y,
    480,
    320 - SPLASH_STATUS_Y,
    theme.bg
  );

  uiApplyTypography(
    TYPE_STATUS,
    theme.wifiGood
  );

  tft.setTextDatum(MC_DATUM);

  tft.drawString(
    "Ready",
    240,
    SPLASH_STATUS_Y + 12
  );

  // Give visual confirmation before prompting the user.
  delay(2500);

  tft.fillRect(
    0,
    SPLASH_STATUS_Y,
    480,
    320 - SPLASH_STATUS_Y,
    theme.bg
  );

  uiApplyTypography(
    TYPE_STATUS,
    theme.textPrimary
  );

  tft.setTextDatum(MC_DATUM);

  tft.drawString(
    "Touch anywhere to continue",
    240,
    SPLASH_STATUS_Y + 12
  );
}