#include "ConexionWiFi.h"

void ConectarWiFi() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long t0 = millis();
  unsigned long ultimoIntento = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    if (millis() - ultimoIntento >= 300) {
      ultimoIntento = millis();
      yield();
    }
  }
  if (WiFi.status() == WL_CONNECTED) Serial.println(WiFi.localIP());
}
