#include "Config.h"
#include "Globals.h"

#include "CtrlTanque.h"
#include "CtrlServo.h"
#include "SensorUltrasonico.h"
#include "ClienteMQTT.h"
#include "ConexionWiFi.h"
#include "ServidorWeb.h"

void setup() {
  CONFIG_MOTOR_UNO
  CONFIG_MOTOR_DOS

  CONFIG_SERVO

  CONFIG_SENSOR_ULTRASONICO

  ConectarWiFi();
  ConfigurarClienteMQTT();
  ConfigurarServidorWeb();
  ultimoMovimientoServo = millis();
}

void loop() {
  ActualizarClienteMQTT();
  ActualizarServidorWeb();
  ActualizarControlServo();
  ProcesarSensorUltrasonico();
  //DistanciaSensorUltrasonico almacenar este valor en una variable global asi no se llama a la funcion
  if (DistanciaSensorUltrasonico() >= 0) EnviarMedicionMQTT();
}
