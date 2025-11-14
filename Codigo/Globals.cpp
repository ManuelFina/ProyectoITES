#include "Globals.h"
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

#include "Globals.h"

WiFiClient   clienteWiFi;
PubSubClient brokerMQTT(clienteWiFi);
WebServer    servidorHTTP(80);

Servo servoBarrido;

long         distanciaCm = -1;
int          anguloActual = 90;
volatile Movimiento movimientoActual = PARADO;
