#include "SensorUltrasonico.h"
#include "Config.h"
#include "CtrlServo.h"
#include "Globals.h"

static volatile unsigned long pulsoMicrosegundos = 0;

enum EstadoUltrasonico {
  US_INICIAR_TRIGGER,
  US_FINALIZAR_TRIGGER,
  US_ESPERAR_ECHO_ALTO,
  US_ESPERAR_ECHO_BAJO
};

static volatile EstadoUltrasonico estadoActual = US_INICIAR_TRIGGER;

static unsigned long tiempoInicioTrigger = 0;
static unsigned long tiempoInicioEco     = 0;
static unsigned long tiempoInicioEspera  = 0;

void MedicionUltrasonico() {
  const unsigned long ahora = micros();

  switch (estadoActual) {

    case US_INICIAR_TRIGGER:
      TRIG_ES_ALTO;
      tiempoInicioTrigger = ahora;
      estadoActual = US_FINALIZAR_TRIGGER;
      break;

    case US_FINALIZAR_TRIGGER:
      if (ahora - tiempoInicioTrigger < 10) return;

      TRIG_ES_BAJO;
      tiempoInicioEspera = ahora;
      estadoActual = US_ESPERAR_ECHO_ALTO;
      break;

    case US_ESPERAR_ECHO_ALTO:
      if (!ECHO_ES_ALTO) {
        if (ahora - tiempoInicioEspera > 30000) { 
          distanciaCm   = -1;              
          estadoActual  = US_INICIAR_TRIGGER;
        }
        return;
      }
      tiempoInicioEco = ahora;
      estadoActual    = US_ESPERAR_ECHO_BAJO;
      break;

    case US_ESPERAR_ECHO_BAJO:
      if (!ECHO_ES_BAJO) return;
      {
        const unsigned long pulso = ahora - tiempoInicioEco;
        pulsoMicrosegundos = pulso;
        distanciaCm        = pulso * 0.0343f / 2.0f;

        estadoActual = US_INICIAR_TRIGGER;
      }
      break;
  }
}

void ProcesarSensorUltrasonico() {
  MedicionUltrasonico();
}

long DistanciaSensorUltrasonico() {
  return distanciaCm;
}

unsigned long ObtenerPulsoUltrasonicoUS() {
  return pulsoMicrosegundos;
}
