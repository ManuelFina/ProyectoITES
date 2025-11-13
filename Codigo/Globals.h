#ifndef GLOBALS_H
#define GLOBALS_H

#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// ---- Tipos compartidos ----
enum Movimiento { PARADO, ADELANTE, ATRAS, IZQ, DER };

// ---- Objetos/vars globales (solo declaración) ----
extern WiFiClient   clienteWiFi;
extern PubSubClient brokerMQTT;
extern WebServer    servidorHTTP;


extern Servo        servoBarrido;  // ← declaración visible globalmente
extern long         distanciaCm;
extern int          anguloActual;
extern volatile Movimiento movimientoActual;

#endif
