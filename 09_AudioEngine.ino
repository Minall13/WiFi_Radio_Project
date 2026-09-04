// ============================================================
// 09_AudioEngine.ino
// Audio Engine
// ============================================================
//
// Purpose:
//
// Provides audio playback services for the application.
//
// Responsibilities:
//
//   • Initialise the audio subsystem.
//   • Control stream playback.
//   • Manage volume and mute.
//   • Service the audio processing loop.
//
// The Audio Engine is independent of the user interface and
// station catalogue.
//
// ============================================================


#include <Audio.h>
#include <esp_heap_caps.h>

void audioInfoCallback(Audio::msg_t message);

Audio audio;

static uint8_t currentVolume = 14;
String currentStreamTitle = "";
bool streamTitleChanged = false;

// ============================================================
// Metadata Callback
// ============================================================

void audioInfoCallback(Audio::msg_t message) {

  if (message.e != Audio::evt_streamtitle) {
    return;
  }

  String newTitle = message.msg;
  newTitle.trim();

  if (
    newTitle.length() < 3 ||
    newTitle.startsWith("HTTP/") ||
    newTitle == "_" ||
    newTitle == "-"
  ) {
    return;
  }

  currentStreamTitle = newTitle;
  streamTitleChanged = true;

  Serial.print("Stream title: ");
  Serial.println(message.msg);
}

// ============================================================
// Initialisation
// ============================================================

bool audioBegin() {
  Serial.println("Audio engine starting");

  Audio::audio_info_callback = audioInfoCallback;

  // Reduce I²S DMA descriptors to preserve internal RAM for
  // Wi-Fi and TLS operations. The library default is 32.
  audio.settings.DMA_DESC_NUM = 16;

  Serial.print("I2S DMA descriptors: ");
  Serial.println(audio.settings.DMA_DESC_NUM);

  audio.setPinout(I2S_BCLK, I2S_LRCK, I2S_DOUT);
  audio.setVolume(currentVolume);

  Serial.print("Audio volume: ");
  Serial.println(currentVolume);

  Serial.print("Internal heap after audio initialisation: ");
  Serial.println(
    heap_caps_get_free_size(
      MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    )
  );

  Serial.print("Largest internal block after audio initialisation: ");
  Serial.println(
    heap_caps_get_largest_free_block(
      MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    )
  );

  Serial.println("Audio engine ready");
  return true;
}

// ============================================================
// Playback
// ============================================================

bool audioPlay(const String& url) {

  currentStreamTitle = "";
  streamTitleChanged = false;

  Serial.println();
  Serial.println("========================================");
  Serial.println("Audio play request:");
  Serial.println(url);

  Serial.println("Stopping previous stream...");
  audio.stopSong();
  Serial.println("Previous stream stopped");

  Serial.print("Total free heap: ");
  Serial.println(ESP.getFreeHeap());

  Serial.print("Internal free heap: ");
  Serial.println(
    heap_caps_get_free_size(
      MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    )
  );

  Serial.print("Largest internal block: ");
  Serial.println(
    heap_caps_get_largest_free_block(
      MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    )
  );

  Serial.print("Minimum free heap since boot: ");
  Serial.println(ESP.getMinFreeHeap());

  unsigned long connectStarted = millis();

  Serial.println("Calling connecttohost...");

  bool connected = audio.connecttohost(url.c_str());

  unsigned long connectDuration =
    millis() - connectStarted;

  Serial.print("connecttohost returned: ");
  Serial.println(
    connected ? "true" : "false"
  );

  Serial.print("Connection attempt duration: ");
  Serial.print(connectDuration);
  Serial.println(" ms");

  Serial.print("Total free heap after connect: ");
  Serial.println(ESP.getFreeHeap());

  Serial.print("Internal free heap after connect: ");
  Serial.println(
    heap_caps_get_free_size(
      MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    )
  );

  Serial.print("Largest internal block after connect: ");
  Serial.println(
    heap_caps_get_largest_free_block(
      MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    )
  );

  Serial.println("========================================");
  Serial.println();

  audioIsPlaying = connected;

  return connected;
}

void audioStop() {

  Serial.println("Audio stop");

  audio.stopSong();
  audioIsPlaying = false;
}

void audioLoop() {
  audio.loop();
}

// ============================================================
// Volume
// ============================================================

void audioSetVolume(uint8_t volume) {
  currentVolume = constrain(volume, 0, 21);
  audio.setVolume(currentVolume);

  Serial.print("Audio volume: ");
  Serial.println(currentVolume);
}

uint8_t audioGetVolume() {
  return currentVolume;
}