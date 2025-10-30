#include "config.h"
#include "globals.h"

#include "CtrlTanque.h"
#include "ControlServo.h"
#include "SensorUltrasonico.h"
#include "ClienteMQTT.h"
#include "ConexionWiFi.h"
#include "ServidorWeb.h"

void setup() {
  CONFIG_MOTOR_UNO
  CONFIG_MOTOR_DOS
  
  //sensorUltrasonico.configurarPines();
  controlServo.configurar();

  conexionWiFi.conectar();
  ConfigurarclienteMQTT();
  ConfigurarservidorWeb();
  ultimoMovimientoServo = millis();
}

void loop() {
  ActualizarclienteMQTT();
  ActualizarservidorWeb();
  /**/
  controlServo.actualizar();
  sensorUltrasonico.procesar();
  ControlMotores.actualizar();
  
  if (sensorUltrasonico.obtenerDistancia() >= 0) clienteMQTT.enviarMedicion();
}
