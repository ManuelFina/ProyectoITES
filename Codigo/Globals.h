#pragma once
#include "Config.h"

extern WiFiClient   clienteWiFi;
extern PubSubClient brokerMQTT;
extern WebServer    servidorHTTP;

// === Variables de estado compartidas ===
extern int           velocidadBase;
extern int           anguloServo;
extern int           direccionServo;
extern unsigned long ultimoMovimientoServo;
extern long          distanciaCm;

// === Instancias globales ===
extern ControlMotores     controlMotores;
extern ControlServo       controlServo;
extern SensorUltrasonico  sensorUltrasonico;
extern ClienteMQTT        clienteMQTT;
extern ConexionWiFi       conexionWiFi;
extern ServidorWeb        servidorWeb;
