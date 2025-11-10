#pragma once
#include "Config.h"

extern WiFiClient   clienteWiFi;
extern PubSubClient brokerMQTT;
extern WebServer    servidorHTTP;

extern int           velocidadBase;
extern int           anguloServo;
extern int           direccionServo;
extern unsigned long ultimoMovimientoServo;
extern long          distanciaCm;
extern volatile Movimiento  movimientoActual;


