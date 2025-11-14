#ifndef GLOBALS_H
#define GLOBALS_H

#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

enum Movimiento { PARADO, ADELANTE, ATRAS, IZQ, DER };

extern WiFiClient   clienteWiFi;
extern PubSubClient brokerMQTT;
extern WebServer    servidorHTTP;


extern Servo        servoBarrido;  
extern long         distanciaCm;
extern int          anguloActual;
extern volatile Movimiento movimientoActual;

#endif
