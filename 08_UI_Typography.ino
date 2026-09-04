// ============================================================
// 08_UI_Typography.ino
// User Interface Typography
// ============================================================
//
// Purpose:
//
// Defines the typography used throughout the application.
//
// Responsibilities:
//
//   • Font selection.
//   • Text styles.
//   • Shared typography helpers.
//   • Consistent text rendering.
//
// This module centralises all typography decisions to ensure
// a consistent visual language.
//
// ============================================================

const bool UI_USE_SMOOTH_FONTS = true;

bool uiSmoothFontsAvailable = false;
bool uiSmoothFontLoaded = false;
UITypeStyle currentTypeface = TYPE_STATUS;

const char* uiFontPath(UITypeStyle style) {

  switch (style) {

    case TYPE_DISPLAY: return "Roboto40";
    case TYPE_HEADER:  return "Roboto36";
    case TYPE_SECTION: return "Roboto32";
    case TYPE_BODY:    return "Roboto28";
    case TYPE_BUTTON:  return "Roboto28";
    case TYPE_CAPTION: return "Roboto24";
    case TYPE_STATUS:  return "Roboto24";
  }

  return "C12";
}

void uiLoadSmoothFont(const char* fontName) {

  tft.unloadFont();
  tft.loadFont(String("/fonts/") + fontName, LittleFS);

  uiSmoothFontLoaded = true;
}

void uiApplyTypography(UITypeStyle style, uint16_t colour) {

  // Background-aware rendering
  tft.setTextColor(colour, theme.bg);

  tft.setTextDatum(TL_DATUM);
  tft.setTextPadding(0);

  if (UI_USE_SMOOTH_FONTS && uiSmoothFontsAvailable) {

    if (!uiSmoothFontLoaded || currentTypeface != style) {

      uiLoadSmoothFont(uiFontPath(style));
      currentTypeface = style;
    }

    return;
  }

  // Bitmap fallback

  switch (style) {

    case TYPE_DISPLAY:
    case TYPE_HEADER:
      tft.setTextFont(4);
      break;

    case TYPE_SECTION:
    case TYPE_BODY:
    case TYPE_BUTTON:
      tft.setTextFont(2);
      break;

    case TYPE_CAPTION:
    case TYPE_STATUS:
      tft.setTextFont(1);
      break;
  }

  tft.setTextSize(1);
}

void uiInitTypography() {

  uiSmoothFontsAvailable =
      LittleFS.exists("/fonts/Roboto24.vlw") &&
      LittleFS.exists("/fonts/Roboto28.vlw") &&
      LittleFS.exists("/fonts/Roboto32.vlw") &&
      LittleFS.exists("/fonts/Roboto36.vlw") &&
      LittleFS.exists("/fonts/Roboto40.vlw");

  Serial.println(
      uiSmoothFontsAvailable ?
      "Smooth fonts available." :
      "Smooth fonts missing. Using bitmap fallback.");
}

void drawFontPreviewScreen() {

  safeTftStart();

  tft.fillScreen(theme.bg);

  setHeader("Typography", "Preview", HEADER_STATUS_NONE);
  drawHeader();

  const int labelX  = 20;
  const int sampleX = 130;

  int y = 72;

  // ---------------------------------------------------------
  // Header
  // ---------------------------------------------------------

  uiApplyTypography(TYPE_CAPTION, theme.textMuted);
  tft.setCursor(labelX, y);
  tft.print("Header");

  uiApplyTypography(TYPE_HEADER, theme.textPrimary);
  tft.setCursor(sampleX, y);
  tft.print("Internet Radio");

  y += 34;

  // ---------------------------------------------------------
  // Section
  // ---------------------------------------------------------

  uiApplyTypography(TYPE_CAPTION, theme.textMuted);
  tft.setCursor(labelX, y);
  tft.print("Section");

  uiApplyTypography(TYPE_SECTION, theme.textPrimary);
  tft.setCursor(sampleX, y);
  tft.print("BBC National");

  y += 32;

  // ---------------------------------------------------------
  // Body
  // ---------------------------------------------------------

  uiApplyTypography(TYPE_CAPTION, theme.textMuted);
  tft.setCursor(labelX, y);
  tft.print("Body");

  uiApplyTypography(TYPE_BODY, theme.textPrimary);
  tft.setCursor(sampleX, y);
  tft.print("BBC Radio 1 Dance");

  y += 32;

  // ---------------------------------------------------------
  // Caption
  // ---------------------------------------------------------

  uiApplyTypography(TYPE_CAPTION, theme.textMuted);
  tft.setCursor(labelX, y);
  tft.print("Caption");

  uiApplyTypography(TYPE_CAPTION, theme.textSecondary);
  tft.setCursor(sampleX, y);
  tft.print("128 kbps • AAC");

  y += 24;

  // ---------------------------------------------------------
  // Status
  // ---------------------------------------------------------

  uiApplyTypography(TYPE_CAPTION, theme.textMuted);
  tft.setCursor(labelX, y);
  tft.print("Status");

  uiApplyTypography(TYPE_STATUS, theme.accent);
  tft.setCursor(sampleX, y);
  tft.print("Connected via WiFi");

  y += 32;

  // ---------------------------------------------------------
  // Display
  // ---------------------------------------------------------

  uiApplyTypography(TYPE_DISPLAY, theme.textPrimary);
  tft.setCursor(sampleX, y);
  tft.print("12:48");

  safeTftEnd();
}