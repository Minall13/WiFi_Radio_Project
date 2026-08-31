// ============================================================
// 15_Station_Artwork.ino
// Station Artwork
// ============================================================
//
// Purpose:
//
// Loads and displays station artwork.
//
// Responsibilities:
//
//   • Load station artwork from LittleFS.
//   • Render PNG station logos.
//   • Display the default artwork when required.
//
// Artwork generation is performed by the external Python build
// tools and is independent of the firmware.
//
// ============================================================


#include <LittleFS.h>
#include <PNGdec.h>

PNG stationPng;
File stationArtworkFile;

static int stationArtworkX = 0;
static int stationArtworkY = 0;
static uint32_t stationArtworkBackground = 0;

void* stationArtworkOpen(const char* path, int32_t* size) {
  stationArtworkFile = LittleFS.open(path, "r");

  if (!stationArtworkFile) {
    return nullptr;
  }

  *size = stationArtworkFile.size();
  return &stationArtworkFile;
}

void stationArtworkClose(void* handle) {
  File* file = static_cast<File*>(handle);

  if (file) {
    file->close();
  }
}

int32_t stationArtworkRead(
  PNGFILE* handle,
  uint8_t* buffer,
  int32_t length
) {
  File* file = static_cast<File*>(handle->fHandle);
  return file->read(buffer, length);
}

int32_t stationArtworkSeek(
  PNGFILE* handle,
  int32_t position
) {
  File* file = static_cast<File*>(handle->fHandle);
  return file->seek(position);
}

int stationArtworkDraw(PNGDRAW* draw) {
  static uint16_t lineBuffer[190];

  stationPng.getLineAsRGB565(
    draw,
    lineBuffer,
    PNG_RGB565_BIG_ENDIAN,
    stationArtworkBackground
  );

  tft.pushImage(
    stationArtworkX,
    stationArtworkY + draw->y,
    draw->iWidth,
    1,
    lineBuffer
  );

  return 1;
}

uint32_t rgb565ToRgb888(uint16_t colour) {
  uint8_t red = (colour >> 11) & 0x1F;
  uint8_t green = (colour >> 5) & 0x3F;
  uint8_t blue = colour & 0x1F;

  red = (red << 3) | (red >> 2);
  green = (green << 2) | (green >> 4);
  blue = (blue << 3) | (blue >> 2);

  return (
    static_cast<uint32_t>(red) << 16
    | static_cast<uint32_t>(green) << 8
    | blue
  );
}

bool drawStationArtwork(
  const char* path,
  int x,
  int y,
  uint16_t backgroundColour
) {
  stationArtworkX = x;
  stationArtworkY = y;
  stationArtworkBackground = rgb565ToRgb888(backgroundColour);

  int result = stationPng.open(
    path,
    stationArtworkOpen,
    stationArtworkClose,
    stationArtworkRead,
    stationArtworkSeek,
    stationArtworkDraw
  );

  if (result != PNG_SUCCESS) {
    Serial.print("Station artwork open failed: ");
    Serial.print(path);
    Serial.print(" (");
    Serial.print(result);
    Serial.println(")");
    return false;
  }

  result = stationPng.decode(nullptr, 0);
  stationPng.close();

  if (result != PNG_SUCCESS) {
    Serial.print("Station artwork decode failed: ");
    Serial.print(path);
    Serial.print(" (");
    Serial.print(result);
    Serial.println(")");
    return false;
  }

  return true;
}