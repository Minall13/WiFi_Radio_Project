// ============================================================
// 02_Display_UI.ino
// Display Utility Functions
// ============================================================
//
// Purpose:
//
// Provides shared display helper functions used throughout the
// application.
//
// Responsibilities:
//
//   • Manage safe TFT access.
//   • Draw common UI controls.
//   • Provide shared display utilities.
//
// This module intentionally contains no application logic.
//
// ============================================================


void safeTftStart() {
  digitalWrite(TOUCH_CS, HIGH);
  delayMicroseconds(10);
}

void safeTftEnd() {
  digitalWrite(TOUCH_CS, HIGH);
  digitalWrite(TFT_CS_PIN, HIGH);
  delayMicroseconds(10);
}

bool insideButton(int x, int y, Button b) {
  return x >= b.x && x <= b.x + b.w &&
         y >= b.y && y <= b.y + b.h;
}

void drawButton(Button b, bool pressed) {
  safeTftStart();

  uint16_t fillColour    = pressed ? TFT_WHITE : theme.buttonBg;
  uint16_t borderColour  = pressed ? TFT_BLACK : theme.buttonBorder;
  uint16_t textColour    = pressed ? TFT_BLACK : theme.buttonText;

  tft.fillRoundRect(b.x, b.y, b.w, b.h, 10, fillColour);

  tft.drawRoundRect(b.x, b.y, b.w, b.h, 10, borderColour);
  tft.drawRoundRect(b.x + 1, b.y + 1, b.w - 2, b.h - 2, 9, borderColour);

  tft.setTextColor(textColour, fillColour);
  tft.setTextSize(2);

  int textX = b.x + 38;
  int textY = b.y + 18;

  if (strlen(b.label) > 0) {
    tft.setCursor(textX, textY);
    tft.print(b.label);
  }

  safeTftEnd();
}

void drawFooterBackButton(bool pressed) {

  drawButton(backFooterButton, pressed);

  safeTftStart();

  uint16_t colour = pressed ? TFT_BLACK : TFT_WHITE;

  int cx = backFooterButton.x + 22;
  int cy = backFooterButton.y + 22;

  tft.fillTriangle(
      cx - 8, cy,
      cx + 4, cy - 8,
      cx + 4, cy + 8,
      colour);

  safeTftEnd();
}

void drawFooterSettingsButton(bool pressed) {

  drawButton(settingsFooterButton, pressed);

  safeTftStart();

  uint16_t colour = pressed ? TFT_BLACK : TFT_WHITE;

  int x1 = settingsFooterButton.x + 12;
  int x2 = settingsFooterButton.x + 33;

  int y1 = settingsFooterButton.y + 15;
  int y2 = settingsFooterButton.y + 22;
  int y3 = settingsFooterButton.y + 29;

  tft.drawLine(x1, y1, x2, y1, colour);
  tft.drawLine(x1, y2, x2, y2, colour);
  tft.drawLine(x1, y3, x2, y3, colour);

  tft.fillCircle(settingsFooterButton.x + 18, y1, 3, colour);
  tft.fillCircle(settingsFooterButton.x + 27, y2, 3, colour);
  tft.fillCircle(settingsFooterButton.x + 22, y3, 3, colour);

  safeTftEnd();
}

void drawStationSelectButton(Button b, int number, bool enabled, bool selected) {
  safeTftStart();

  uint16_t outlineColour;
  uint16_t textColour;
  uint16_t glowColour;

  if (!enabled) {
    outlineColour = tft.color565(70, 70, 70);
    textColour = tft.color565(90, 90, 90);
    glowColour = TFT_BLACK;
  }
  else if (selected) {
    outlineColour = tft.color565(0, 90, 220);
    textColour = TFT_BLACK;
    glowColour = tft.color565(0, 90, 220);
  }
  else {
    outlineColour = tft.color565(0, 90, 220);
    textColour = tft.color565(0, 90, 220);
    glowColour = TFT_BLACK;
  }

  tft.fillRoundRect(b.x, b.y, b.w, b.h, 10, TFT_BLACK);

  if (selected) {
    // Selected button uses the accent colour fill
    tft.fillRoundRect(b.x + 4, b.y + 4, b.w - 8, b.h - 8, 8, glowColour);
  }

  tft.drawRoundRect(b.x, b.y, b.w, b.h, 10, outlineColour);
  tft.drawRoundRect(b.x + 1, b.y + 1, b.w - 2, b.h - 2, 9, outlineColour);

  uiApplyTypography(TYPE_BUTTON, textColour);
  tft.setTextColor(textColour, selected ? glowColour : TFT_BLACK);

  String numberText = String(number);
  int textX = b.x + (b.w - tft.textWidth(numberText)) / 2 - 2;
  int textY = b.y + 6;

  tft.setCursor(textX, textY);
  tft.print(numberText);

  safeTftEnd();
}

void drawFilterMenuButton(bool pressed) {
  safeTftStart();

  uint16_t fillColour = pressed ? TFT_WHITE : TFT_DARKGREY;
  uint16_t textColour = pressed ? TFT_BLACK : TFT_WHITE;

  tft.fillRoundRect(filterMenuButton.x, filterMenuButton.y,
                    filterMenuButton.w, filterMenuButton.h,
                    10, fillColour);

  tft.drawRoundRect(filterMenuButton.x, filterMenuButton.y,
                    filterMenuButton.w, filterMenuButton.h,
                    10, TFT_WHITE);

  tft.setTextColor(textColour, fillColour);
  tft.setTextSize(2);
  tft.setCursor(filterMenuButton.x + 18, filterMenuButton.y + 14);
  tft.print(stationFilters[currentStationFilter]);

  int cx = filterMenuButton.x + filterMenuButton.w - 22;
  int cy = filterMenuButton.y + 20;

  tft.fillTriangle(
    cx - 7, cy - 4,
    cx + 7, cy - 4,
    cx,     cy + 6,
    textColour
  );

  safeTftEnd();
}

void drawFilterChoiceButton(Button b, bool selected) {
  safeTftStart();

  uint16_t fillColour = selected ? TFT_WHITE : TFT_DARKGREY;
  uint16_t textColour = selected ? TFT_BLACK : TFT_WHITE;

  tft.fillRoundRect(b.x, b.y, b.w, b.h, 10, fillColour);
  tft.drawRoundRect(b.x, b.y, b.w, b.h, 10, TFT_WHITE);
  tft.drawRoundRect(b.x + 1, b.y + 1, b.w - 2, b.h - 2, 9, TFT_WHITE);

  uiApplyTypography(TYPE_BUTTON, textColour);
  tft.setTextColor(textColour, fillColour);

  int textX = b.x + (b.w - tft.textWidth(b.label)) / 2;
  int textY = b.y + 8;

  tft.setCursor(textX, textY);
  tft.print(b.label);

  safeTftEnd();
}

void drawArrowButton(Button b, bool pressed, bool upArrow) {
  safeTftStart();

  uint16_t fillColour = pressed ? TFT_WHITE : TFT_DARKGREY;
  uint16_t arrowColour = pressed ? TFT_BLACK : TFT_WHITE;

  tft.fillRoundRect(b.x, b.y, b.w, b.h, 10, fillColour);
  tft.drawRoundRect(b.x, b.y, b.w, b.h, 10, TFT_WHITE);
  tft.drawRoundRect(b.x + 1, b.y + 1, b.w - 2, b.h - 2, 9, TFT_WHITE);

  int cx = b.x + b.w / 2;
  int cy = b.y + b.h / 2;

  if (upArrow) {
    tft.fillTriangle(
      cx, cy - 12,
      cx - 12, cy + 10,
      cx + 12, cy + 10,
      arrowColour
    );
  }
  else {
    tft.fillTriangle(
      cx, cy + 12,
      cx - 12, cy - 10,
      cx + 12, cy - 10,
      arrowColour
    );
  }

  safeTftEnd();
}

void drawFilterSettingsButton(bool pressed) {
  safeTftStart();

  uint16_t fillColour =
    pressed ? theme.textMuted : theme.buttonBg;

  uint16_t accentColour =
    pressed ? TFT_BLACK : theme.textMuted;

  tft.fillRoundRect(
    filterSettingsButton.x,
    filterSettingsButton.y,
    filterSettingsButton.w,
    filterSettingsButton.h,
    10,
    fillColour
  );

  tft.drawRoundRect(
    filterSettingsButton.x,
    filterSettingsButton.y,
    filterSettingsButton.w,
    filterSettingsButton.h,
    10,
    accentColour
  );

  tft.drawRoundRect(
    filterSettingsButton.x + 1,
    filterSettingsButton.y + 1,
    filterSettingsButton.w - 2,
    filterSettingsButton.h - 2,
    9,
    accentColour
  );

  uiApplyTypography(
    TYPE_BUTTON,
    accentColour
  );

  tft.setTextColor(
    accentColour,
    fillColour
  );

  int textX =
    filterSettingsButton.x +
    (
      filterSettingsButton.w -
      tft.textWidth(filterSettingsButton.label)
    ) / 2;

  int textY =
    filterSettingsButton.y + 5;

  tft.setCursor(
    textX,
    textY
  );

  tft.print(
    filterSettingsButton.label
  );

  safeTftEnd();
}

void showMessage(const char* message) {
  Serial.print("showMessage: ");
  Serial.println(message);
}

void handleButtonPress(Button b, const char* message) {
  Serial.println(message);

  drawButton(b, true);
  delay(150);
  drawButton(b, false);

  pendingMessage = message;
  messagePending = true;
}

void setHeader(const String& title,
               const String& centre,
               uint8_t status)
{
    headerTitle  = title;
    headerCentre = centre;
    headerStatus = status;
}

void drawHeader() {
  uint16_t headerBg = theme.headerBg;
  uint16_t textMain = theme.textPrimary;
  uint16_t textSub  = theme.textSecondary;

  uint16_t wifiOk   = theme.wifiGood;
  uint16_t wifiBad  = theme.wifiOff;

  tft.fillRect(0, HEADER_Y, 480, HEADER_H, headerBg);

  // Left: context/title
  uiApplyTypography(TYPE_HEADER, textMain);
  tft.setCursor(18, 17);
  tft.print(headerTitle);

  // Centre: state / page indicator
  if (headerCentre.length() > 0) {
    uiApplyTypography(TYPE_CAPTION, textSub);
    tft.setTextColor(textSub, headerBg);

    int x = (480 - tft.textWidth(headerCentre)) / 2;
    tft.setCursor(x, 23);
    tft.print(headerCentre);
  }

  // Right: status
  if (headerStatus == HEADER_STATUS_WIFI) {
    drawIcon(WiFi.status() == WL_CONNECTED ? ICON_WIFI_ON : ICON_WIFI_OFF,
         432, 16,
         ICON_SMALL);
  }
}

void drawContentArea() {
  tft.fillRect(0, CONTENT_Y, 480, CONTENT_H, theme.bg);
}

void drawFooter() {
  tft.fillRect(0, FOOTER_Y, 480, FOOTER_H, theme.footerBg);
}

void drawFooterLayout() {
  drawFooterBackButton(false);

  int stationIndex1 = stationPageStart + 0;
  int stationIndex2 = stationPageStart + 1;
  int stationIndex3 = stationPageStart + 2;
  int stationIndex4 = stationPageStart + 3;
  int stationIndex5 = stationPageStart + 4;

  drawStationSelectButton(stationSelectButton1, 1, stationIndex1 < stationCount, stationIndex1 == selectedStationIndex);
  drawStationSelectButton(stationSelectButton2, 2, stationIndex2 < stationCount, stationIndex2 == selectedStationIndex);
  drawStationSelectButton(stationSelectButton3, 3, stationIndex3 < stationCount, stationIndex3 == selectedStationIndex);
  drawStationSelectButton(stationSelectButton4, 4, stationIndex4 < stationCount, stationIndex4 == selectedStationIndex);
  drawStationSelectButton(stationSelectButton5, 5, stationIndex5 < stationCount, stationIndex5 == selectedStationIndex);

  drawFooterSettingsButton(false);
}

void drawScreenFramework() {
  drawHeader();
  drawContentArea();
  drawFooter();
  drawFooterLayout();
}


int getStationIndexFromTouch(int screenY) {
  int firstRowY = 75;
  int rowHeight = 32;

  if (screenY < firstRowY) {
    return -1;
  }

  int row = (screenY - firstRowY) / rowHeight;

  if (row < 0 || row >= STATIONS_PER_PAGE) {
    return -1;
  }

  int stationIndex = stationPageStart + row;

  if (stationIndex >= stationCount) {
    return -1;
  }

  return stationIndex;
}
