#include "ClienteMQTT.h"

void ClienteMQTT::configurar() {
  brokerMQTT.setServer(MQTT_BROKER, MQTT_PORT);
  reconectar();
}

void ClienteMQTT::reconectar() {
  while (!brokerMQTT.connected()) {
    if (brokerMQTT.connect(MQTT_CLIENT_ID)) break;
    brokerMQTT.loop();
    yield();
  }
}

void ClienteMQTT::actualizar() {
  if (!brokerMQTT.connected()) reconectar();
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
