// ============================================================
// 09_UI_Icons.ino
// User Interface Icons
// ============================================================
//
// Purpose:
//
// Draws the vector icons used throughout the application.
//
// Responsibilities:
//
//   • Render common UI symbols.
//   • Provide reusable icon drawing functions.
//   • Maintain a consistent visual style.
//
// Icons are rendered using TFT graphics and are independent
// of application logic.
//
// ============================================================


#include <LittleFS.h>

void drawRaw565Icon(const char* path, int x, int y, int w, int h) {
  File f = LittleFS.open(path, "r");

  if (!f) {
    Serial.print("Icon not found: ");
    Serial.println(path);
    return;
  }

  uint16_t buffer[w];

  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      int hi = f.read();
      int lo = f.read();

      if (hi < 0 || lo < 0) {
        f.close();
        Serial.print("Icon read error: ");
        Serial.println(path);
        return;
      }

      buffer[col] = (lo << 8) | hi;
    }

    tft.pushImage(x, y + row, w, 1, buffer);
  }

  f.close();
}

struct IconDefinition {
  const char* name;
  const char* colour;
};

const IconDefinition iconTable[] = {
  { "wifi_on",    "green" },
  { "wifi_off",   "amber" },

  { "catalogue",  "blue" },
  { "stations",   "blue" },

  { "router_on",  "green" },
  { "router_off", "red" },

  { "back",       "blue" },
  { "settings",   "blue" }
};

void drawIcon(IconId icon, int x, int y, IconSize size) {
  const char* iconName = iconTable[icon].name;
  const char* iconColour = iconTable[icon].colour;

  char path[48];
  int iconPixels = 48;

  switch (size) {
    case ICON_SMALL:
      iconPixels = 24;
      break;

    case ICON_MEDIUM:
      iconPixels = 32;
      break;

    case ICON_LARGE:
      iconPixels = 48;
      break;
  }

  if (iconColour != nullptr) {
    snprintf(path, sizeof(path), "/icons/%s_%s_%d.raw", iconName, iconColour, iconPixels);
  } else {
    snprintf(path, sizeof(path), "/icons/%s_%d.raw", iconName, iconPixels);
  }

  drawRaw565Icon(path, x, y, iconPixels, iconPixels);
}