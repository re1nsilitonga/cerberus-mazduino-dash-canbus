// Minimal TFT_eSPI test for ILI9488 480x320 wiring/driver isolation.
// Build & upload with: pio run -e tft_test -t upload
#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("TFT minimal test starting...");

  tft.init();
  tft.setRotation(3);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("TFT_eSPI Test", 10, 10);
  tft.drawString("ILI9488 480x320", 10, 40);

  tft.drawRect(10, 100, 100, 50, TFT_RED);
  tft.fillRect(120, 100, 100, 50, TFT_GREEN);
  tft.drawCircle(270, 125, 25, TFT_BLUE);

  Serial.println("TFT initialized and test pattern displayed");
}

void loop() {
  static unsigned long lastUpdate = 0;
  static int idx = 0;
  uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_WHITE, TFT_BLACK};
  const char* names[] = {"RED", "GREEN", "BLUE", "WHITE", "BLACK"};

  if (millis() - lastUpdate > 1500) {
    tft.fillScreen(colors[idx]);
    Serial.printf("fillScreen: %s\n", names[idx]);
    idx = (idx + 1) % 5;
    lastUpdate = millis();
  }
}
