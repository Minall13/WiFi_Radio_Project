// ============================================================
// 06_UI_Theme_Design.ino
// User Interface Theme
// ============================================================
//
// Purpose:
//
// Defines the visual theme used throughout the application.
//
// Responsibilities:
//
//   • Colour palette.
//   • Theme definitions.
//   • Shared UI styling.
//   • Semantic UI colours.
//
// This module contains presentation data only and no
// application logic.
//
// ============================================================


// ============================================================
// THEME: Midnight (Default)
// ============================================================

UITheme themeMidnight = {
  TFT_BLACK,                     // bg
  TFT_BLACK,                     // panel
  tft.color565(18, 24, 32),      // headerBg
  TFT_BLACK,                     // footerBg

  tft.color565(235, 240, 245),   // textPrimary
  tft.color565(180, 190, 200),   // textSecondary
  tft.color565(235, 136, 14),    // textMuted
  tft.color565(70, 145, 255),    // textValue
  TFT_WHITE,                     // textStrong

  tft.color565(0, 90, 220),      // accent
  TFT_DARKGREY,                  // buttonBg
  tft.color565(55, 60, 72),      // playerButtonBg
  TFT_WHITE,                     // buttonBorder
  TFT_WHITE,                     // buttonText

  tft.color565(32, 235, 14),     // wifiGood
  TFT_YELLOW,                    // wifiWeak
  tft.color565(240, 120, 80),    // wifiOff

  tft.color565(0, 120, 255),     // playerBack
  tft.color565(220, 45, 45),     // playerStop
  tft.color565(211, 134, 227),   // playerVolumeDown
  tft.color565(240, 165, 0),     // playerMute
  tft.color565(40, 180, 80)      // playerVolumeUp

};

// ============================================================
// THEME: Graphite
// ============================================================

UITheme themeGraphite = {
  TFT_BLACK,                     // bg
  TFT_BLACK,                     // panel
  tft.color565(40, 40, 40),      // headerBg
  tft.color565(28, 28, 28),      // footerBg

  TFT_WHITE,                     // textPrimary
  tft.color565(175, 175, 175),   // textSecondary
  tft.color565(220, 220, 220),   // textMuted
  tft.color565(205, 205, 205),   // textValue
  TFT_WHITE,                     // textStrong

  tft.color565(150, 150, 150),   // accent
  tft.color565(45, 45, 45),      // buttonBg
  tft.color565(55, 60, 72),      // playerButtonBg
  tft.color565(180, 180, 180),   // buttonBorder
  TFT_WHITE,                     // buttonText

  tft.color565(0, 220, 80),      // wifiGood
  TFT_YELLOW,                    // wifiWeak
  tft.color565(255, 80, 80),     // wifiOff

  tft.color565(0, 120, 255),     // playerBack
  tft.color565(220, 45, 45),     // playerStop
  tft.color565(211, 134, 227),   // playerVolumeDown
  tft.color565(240, 165, 0),     // playerMute
  tft.color565(40, 180, 80)      // playerVolumeUp

};

// ============================================================
// THEME: Ocean
// ============================================================

UITheme themeOcean = {
  TFT_BLACK,                     // bg
  tft.color565(5, 15, 25),       // panel
  tft.color565(10, 40, 70),      // headerBg
  tft.color565(0, 30, 50),       // footerBg

  TFT_WHITE,                     // textPrimary
  tft.color565(120, 220, 255),   // textSecondary
  tft.color565(180, 240, 255),   // textMuted
  tft.color565(90, 205, 255),    // textValue
  TFT_WHITE,                     // textStrong

  tft.color565(0, 170, 255),     // accent
  tft.color565(15, 45, 70),      // buttonBg
  tft.color565(55, 60, 72),      // playerButtonBg
  tft.color565(80, 180, 255),    // buttonBorder
  TFT_WHITE,                     // buttonText

  tft.color565(0, 255, 140),     // wifiGood
  tft.color565(255, 220, 0),     // wifiWeak
  tft.color565(255, 100, 100),   // wifiOff

  tft.color565(0, 120, 255),     // playerBack
  tft.color565(220, 45, 45),     // playerStop
  tft.color565(211, 134, 227),   // playerVolumeDown
  tft.color565(240, 165, 0),     // playerMute
  tft.color565(40, 180, 80)      // playerVolumeUp

};

// ============================================================
// THEME: Amber
// ============================================================

UITheme themeAmber = {
  TFT_BLACK,                     // bg
  TFT_BLACK,                     // panel
  tft.color565(40, 20, 0),       // headerBg
  tft.color565(28, 14, 0),       // footerBg

  tft.color565(255, 210, 120),   // textPrimary
  tft.color565(255, 180, 70),    // textSecondary
  tft.color565(255, 230, 150),   // textMuted
  tft.color565(255, 195, 95),    // textValue
  TFT_WHITE,                     // textStrong

  tft.color565(255, 160, 0),     // accent
  tft.color565(35, 20, 0),       // buttonBg
  tft.color565(55, 60, 72),      // playerButtonBg
  tft.color565(255, 170, 40),    // buttonBorder
  tft.color565(255, 210, 120),   // buttonText

  tft.color565(0, 255, 100),     // wifiGood
  tft.color565(255, 210, 0),     // wifiWeak
  tft.color565(255, 80, 80),     // wifiOff

  tft.color565(0, 120, 255),     // playerBack
  tft.color565(220, 45, 45),     // playerStop
  tft.color565(211, 134, 227),   // playerVolumeDown
  tft.color565(240, 165, 0),     // playerMute
  tft.color565(40, 180, 80)      // playerVolumeUp

};

// ============================================================
// THEME: Retro Green
// ============================================================

UITheme themeRetroGreen = {
  TFT_BLACK,                     // bg
  TFT_BLACK,                     // panel
  tft.color565(0, 25, 0),        // headerBg
  tft.color565(0, 18, 0),        // footerBg

  tft.color565(150, 255, 150),   // textPrimary
  tft.color565(90, 220, 90),     // textSecondary
  tft.color565(190, 255, 190),   // textMuted
  tft.color565(120, 245, 120),   // textValue
  TFT_WHITE,                     // textStrong

  tft.color565(0, 220, 0),       // accent
  tft.color565(0, 25, 0),        // buttonBg
  tft.color565(55, 60, 72),      // playerButtonBg
  tft.color565(0, 180, 0),       // buttonBorder
  tft.color565(150, 255, 150),   // buttonText

  tft.color565(0, 255, 120),     // wifiGood
  tft.color565(255, 220, 0),     // wifiWeak
  tft.color565(255, 80, 80),     // wifiOff

  tft.color565(0, 120, 255),     // playerBack
  tft.color565(220, 45, 45),     // playerStop
  tft.color565(211, 134, 227),   // playerVolumeDown
  tft.color565(240, 165, 0),     // playerMute
  tft.color565(40, 180, 80)      // playerVolumeUp

};

// ============================================================
// THEME: Daylight
// ============================================================

UITheme themeDaylight = {
  TFT_WHITE,                     // bg
  tft.color565(245, 245, 245),   // panel
  tft.color565(225, 235, 245),   // headerBg
  tft.color565(215, 225, 235),   // footerBg

  TFT_BLACK,                     // textPrimary
  tft.color565(70, 70, 70),      // textSecondary
  tft.color565(30, 30, 30),      // textMuted
  tft.color565(0, 90, 180),      // textValue
  TFT_WHITE,                     // textStrong

  tft.color565(0, 90, 220),      // accent
  TFT_WHITE,                     // buttonBg
  tft.color565(55, 60, 72),      // playerButtonBg
  tft.color565(120, 120, 120),   // buttonBorder
  TFT_BLACK,                     // buttonText

  tft.color565(0, 180, 70),      // wifiGood
  tft.color565(255, 170, 0),     // wifiWeak
  tft.color565(220, 60, 60),     // wifiOff

  tft.color565(0, 120, 255),     // playerBack
  tft.color565(220, 45, 45),     // playerStop
  tft.color565(211, 134, 227),   // playerVolumeDown
  tft.color565(240, 165, 0),     // playerMute
  tft.color565(40, 180, 80)      // playerVolumeUp

};

// ============================================================
// ACTIVE THEME
// ============================================================

UITheme theme = themeMidnight;
