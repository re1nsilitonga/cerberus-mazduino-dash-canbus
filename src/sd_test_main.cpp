// Minimal SD card test on its own dedicated SPI bus (not shared with TFT),
// to rule out any wiring/solder issue on the shared bus pins.
// Build & upload with: pio run -e sd_test -t upload
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define SD_SCK   13
#define SD_MISO  14
#define SD_MOSI  15
#define SD_CS    16

static const char *TEST_PATH = "/cerberus_sd_test.txt";

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("SD card test starting...");

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  // Slow init speed first (some cards/wiring need this to even handshake)
  if (!SD.begin(SD_CS, SPI, 400000)) {
    Serial.println("SD.begin FAILED at 400kHz! Check wiring/CS pin/card/format(FAT32).");
    while (1) delay(1000);
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card detected.");
    while (1) delay(1000);
  }
  Serial.printf("Card type: %s\n",
                cardType == CARD_MMC ? "MMC" :
                cardType == CARD_SD ? "SDSC" :
                cardType == CARD_SDHC ? "SDHC" : "UNKNOWN");
  Serial.printf("Card size: %llu MB\n", SD.cardSize() / (1024 * 1024));
  Serial.printf("Used space: %llu MB\n", SD.usedBytes() / (1024 * 1024));

  Serial.println("Listing root directory:");
  File root = SD.open("/");
  File entry;
  while ((entry = root.openNextFile())) {
    Serial.printf("  %s%s  %u bytes\n", entry.isDirectory() ? "[DIR] " : "",
                  entry.name(), entry.size());
    entry.close();
  }
  root.close();

  // Write test
  Serial.println("Writing test file...");
  File f = SD.open(TEST_PATH, FILE_WRITE);
  if (!f) {
    Serial.println("Open for write FAILED!");
    while (1) delay(1000);
  }
  f.println("Cerberus SD test OK");
  f.printf("millis=%lu\n", millis());
  f.close();

  // Read-back verify
  Serial.println("Reading test file back...");
  f = SD.open(TEST_PATH, FILE_READ);
  if (!f) {
    Serial.println("Open for read FAILED!");
    while (1) delay(1000);
  }
  bool sawMarker = false;
  while (f.available()) {
    String line = f.readStringUntil('\n');
    Serial.println(line);
    if (line.indexOf("Cerberus SD test OK") >= 0) sawMarker = true;
  }
  f.close();

  Serial.println(sawMarker ? "RESULT: SD card write/read OK"
                            : "RESULT: SD card read did not match what was written!");
}

void loop() {
  delay(1000);
}
