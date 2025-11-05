#include "ClienteMQTT.h"

void ConfigurarClienteMQTT(); {
  brokerMQTT.setServer(MQTT_BROKER, MQTT_PORT);
  ReconectarMQTT();
}

void ReconectarMQTT() {
  while (!brokerMQTT.connected()) {
    if (brokerMQTT.connect(MQTT_CLIENT_ID)) break;
    brokerMQTT.loop();
    yield();
  }
}

void ActualizarClienteMQTT() {
  if (!brokerMQTT.connected()) ReconectarMQTT();
  brokerMQTT.loop();
}

void EnviarMedicionMQTT() {
  if (!brokerMQTT.connected() || distanciaCm < 0) return;

  String payload = "{";
  payload += "\"device_id\":\"" + String(DEVICE_ID) + "\"";
  payload += ",\"distance_cm\":" + String(distanciaCm, 2);   
  payload += ",\"angle_deg\":" + String(anguloServo, 2);
  payload += ",\"raw_us\":" + String(raw_us);               
  payload += ",\"correlation_id\":\"sim-" + String(millis()) + "\"";
  payload += "}";

  brokerMQTT.publish(MQTT_TOPIC_MEDICIONES, payload.c_str());
}
