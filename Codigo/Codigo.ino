#include "config.h"
#include "globals.h"

#include "ControlMotores.h"
#include "ControlServo.h"
#include "SensorUltrasonico.h"
#include "ClienteMQTT.h"
#include "ConexionWiFi.h"
#include "ServidorWeb.h"
//#include "ConfiguradorPWM.h"

void setup() {
  controlMotores.configurarPines();
  sensorUltrasonico.configurarPines();
  configPWM.configurar();
  controlServo.configurar();
  controlMotores.detener();
  
  conexionWiFi.conectar();
  clienteMQTT.configurar();
  servidorWeb.configurar();}
  ultimoMovimientoServo = millis();
}

void loop() {
  clienteMQTT.actualizar();
  servidorWeb.actualizar();
  controlServo.actualizar();
  sensorUltrasonico.procesar();

  if (sensorUltrasonico.obtenerDistancia() >= 0) clienteMQTT.enviarMedicion();
}
