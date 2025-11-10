#include "globals.h"

#include "CtrlRuedas.h"
#include "CtrlServo.h"
#include "SensorUltrasonico.h"
#include "ClienteMQTT.h"
#include "ConexionWiFi.h"
#include "ServidorWeb.h"

WiFiClient   clienteWiFi;
PubSubClient brokerMQTT(clienteWiFi);
WebServer    servidorHTTP(80);

int           anguloServo = 90;
int           direccionServo = 1;
unsigned long ultimoMovimientoServo = 0;
long          distanciaCm = -1;
volatile Movimiento  movimientoActual = PARADO;
