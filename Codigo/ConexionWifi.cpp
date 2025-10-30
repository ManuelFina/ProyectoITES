#include "ConexionWiFi.h"

void ConectarWiFi() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  const unsigned long TIEMPO_MAXIMO_ESPERA = 15000;  
  const unsigned long INTERVALO_REINTENTO = 300;     

  unsigned long tiempoInicio = millis();
  unsigned long ultimoIntento = 0;

  while (WiFi.status() != WL_CONNECTED && (millis() - tiempoInicio < TIEMPO_MAXIMO_ESPERA)) {
    if (millis() - ultimoIntento >= INTERVALO_REINTENTO) {
      ultimoIntento = millis();
      Serial.print(".");  
      yield();            
    }
  }

  Serial.println();  
  
  if (WiFi.status() == WL_CONNECTED) Serial.println("WiFi conectado correctamente");
  else Serial.println("Error: no se pudo establecer conexión WiFi");
  
}
