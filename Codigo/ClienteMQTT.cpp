#include "ClienteMQTT.h"

void ConfigurarclienteMQTT(); {
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

void ActualizarclienteMQTT() {
  if (!brokerMQTT.connected()) ReconectarMQTT();
  brokerMQTT.loop();
}

void ClienteMQTT::enviarMedicion() {
  if (!brokerMQTT.connected() || distanciaCm < 0) return;

  String payload = "{";
  payload += "\"distancia\":" + String(distanciaCm);
  payload += ",\"angulo\":" + String(anguloServo);
  payload += ",\"ts\":" + String(millis());
  payload += "}";

  brokerMQTT.publish(MQTT_TOPIC_MEDICIONES, payload.c_str());
}
