#include "globals.h"

#include "ControlMotores.h"
#include "ControlServo.h"
#include "SensorUltrasonico.h"
#include "ClienteMQTT.h"
#include "ConexionWiFi.h"
#include "ServidorWeb.h"
#include "ConfiguradorPWM.h"

WiFiClient   clienteWiFi;
PubSubClient brokerMQTT(clienteWiFi);
WebServer    servidorHTTP(80);

int           anguloServo = 90;
int           direccionServo = 1;
unsigned long ultimoMovimientoServo = 0;
long          distanciaCm = -1;
volatile int  Movimiento;


ControlMotores     controlMotores;
ControlServo       controlServo;
SensorUltrasonico  sensorUltrasonico;
ClienteMQTT        clienteMQTT;
ConexionWiFi       conexionWiFi;
ServidorWeb        servidorWeb;
ConfiguradorPWM    configPWM;
