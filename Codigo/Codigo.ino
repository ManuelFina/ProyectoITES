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
  CONFIG_SENSOR_ULTRASONICO

  ConectarWiFi();
  ConfigurarClienteMQTT();
  ConfigurarServidorWeb();
}

void loop() {
  ActualizarClienteMQTT();
  ActualizarServidorWeb();
  MovimientoServo();
  ProcesarSensorUltrasonico();
  //DistanciaSensorUltrasonico almacenar este valor en una variable global asi no se llama a la funcion
  if (DistanciaSensorUltrasonico() >= 0) EnviarMedicionMQTT();
}
