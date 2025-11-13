#include "ClienteMQTT.h"
#include "Globals.h"
#include "Config.h"

static unsigned long proximoReintentoMQTT = 0;
static const unsigned long INTERVALO_REINTENTO_MQTT = 2000; // 2s
static unsigned long ultimoPublish = 0;
static const unsigned long INTERVALO_PUBLISH_MS = 250; // evita flood

void ConfigurarClienteMQTT() {
  brokerMQTT.setServer(MQTT_BROKER, MQTT_PORT);
  brokerMQTT.setCallback(CallbackMQTT);
}

void ReconectarMQTT() {
  if (brokerMQTT.connected()) return;
  unsigned long ahora = millis();
  if (ahora < proximoReintentoMQTT) return;

  proximoReintentoMQTT = ahora + INTERVALO_REINTENTO_MQTT;
  brokerMQTT.connect(MQTT_CLIENT_ID); // 1 intento y salimos
}

void ActualizarClienteMQTT() {
  ReconectarMQTT();
  if (brokerMQTT.connected()) brokerMQTT.loop();
}

void EnviarMedicionMQTT() {
  if (!brokerMQTT.connected()) return;

  unsigned long ahora = millis();
  if (ahora - ultimoPublish < INTERVALO_PUBLISH_MS) return;
  ultimoPublish = ahora;

  // Usa las variables globales calculadas por el sensor/servo
  String payload = "{";
  payload += "\"device_id\":\"" + String(MQTT_CLIENT_ID) + "\"";
  payload += ",\"distance_cm\":" + String(distanciaCm, 2);
  payload += ",\"angle_deg\":" + String(anguloActual);
  payload += ",\"ts\":" + String(ahora);
  payload += "}";

  brokerMQTT.publish(MQTT_TOPIC_MEDICIONES, payload.c_str());
}
