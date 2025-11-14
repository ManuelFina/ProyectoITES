#include "Config.h"
#include "Globals.h"

#include "CtrlRuedas.h"
#include "CtrlServo.h"
#include "SensorUltrasonico.h"
#include "ClienteMQTT.h"
#include "ConexionWiFi.h"
#include "ServidorWeb.h"

void setup() {
  CONFIG_MOTORES
  CONFIG_SERVO
  CONFIG_ULTRASONICO
  CONFIGURAR_PWM
  VELOCIDAD_MAXIMA

  ConectarWiFi();
  ConfigurarServidorWeb();
  ConfigurarClienteMQTT();

  DETENER_MOTORES
}
void loop() {
  ActualizarServidorWeb();
  ActualizarClienteMQTT();
  MovimientoServo();
  ProcesarSensorUltrasonico();
  MovimientoTanque();

  if (DistanciaSensorUltrasonico() >= 0) EnviarMedicionMQTT();
}
