#include "SensorUltrasonico.h"

void SensorUltrasonico::configurarPines() {
  pinMode(PIN_TRIGGER, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIGGER, LOW);
}

void SensorUltrasonico::iniciarTrigger(unsigned long ahora_us) {
  digitalWrite(PIN_TRIGGER, HIGH);
  tInicioTrigger_us = ahora_us;
  estado = US_TRIGGER;
}

void SensorUltrasonico::finalizarTrigger() {
  digitalWrite(PIN_TRIGGER, LOW);
  estado = US_ESPERANDO_ALTO;
}

void SensorUltrasonico::iniciarMedicion(unsigned long ahora_us) {
  tInicioEcho_us = ahora_us;
  estado = US_MIDIENDO;
}

void SensorUltrasonico::finalizarMedicion(unsigned long ahora_us, unsigned long ahora_ms) {
  tFinEcho_us = ahora_us;
  unsigned long duracion = tFinEcho_us - tInicioEcho_us;
  distanciaCm = (long)(duracion * US_VELOCIDAD_SONIDO / 2.0f);
  ultimoDisparoUS_ms = ahora_ms;
  estado = US_INACTIVO;
}

void SensorUltrasonico::procesar() {
  unsigned long ahora_ms = millis();
  unsigned long ahora_us = micros();

  switch (estado) {
    case US_INACTIVO:
      if (ahora_ms - ultimoDisparoUS_ms >= INTERVALO_US_MS) iniciarTrigger(ahora_us);
      break;
    case US_TRIGGER:
      if (ahora_us - tInicioTrigger_us >= DURACION_TRIGGER_US) finalizarTrigger();
      break;
    case US_ESPERANDO_ALTO:
      if (digitalRead(PIN_ECHO) == HIGH) iniciarMedicion(ahora_us);
      break;
    case US_MIDIENDO:
      if (digitalRead(PIN_ECHO) == LOW) finalizarMedicion(ahora_us, ahora_ms);
      break;
    case US_ESPERANDO_BAJO:
      estado = US_INACTIVO;
      break;
  }
}

long SensorUltrasonico::obtenerDistancia() { return distanciaCm; }
