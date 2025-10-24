#pragma once
#include "Config.h"
#include "globals.h"

enum EstadoUS { 
  US_INACTIVO, 
  US_TRIGGER, 
  US_ESPERANDO_ALTO, 
  US_MIDIENDO, 
  US_ESPERANDO_BAJO 
};

class SensorUltrasonico {
public:
  void configurarPines();
  void procesar();
  long obtenerDistancia();

private:
  EstadoUS estado = US_INACTIVO;
  unsigned long ultimoDisparoUS_ms = 0;
  unsigned long tInicioTrigger_us = 0;
  unsigned long tInicioEcho_us = 0;
  unsigned long tFinEcho_us = 0;

  void iniciarTrigger(unsigned long ahora_us);
  void finalizarTrigger();
  void iniciarMedicion(unsigned long ahora_us);
  void finalizarMedicion(unsigned long ahora_us, unsigned long ahora_ms);
};
