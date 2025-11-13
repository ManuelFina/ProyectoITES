#include "SensorUltrasonico.h"
#include "Config.h"
#include "CtrlServo.h"
#include "Globals.h"

// ==============================
//  Variables internas del sensor
// ==============================
static volatile unsigned long pulsoMicrosegundos = 0;
static volatile float  anguloMedido = 0.0f;

// Estados internos del autómata del sensor ultrasónico
enum EstadoUltrasonico {
  US_INICIAR_TRIGGER,
  US_FINALIZAR_TRIGGER,
  US_ESPERAR_ECHO_ALTO,
  US_ESPERAR_ECHO_BAJO
};

// Estado actual de la FSM
static EstadoUltrasonico estadoActual = US_INICIAR_TRIGGER;

// Tiempos de referencia
static unsigned long tiempoInicioTrigger = 0;
static unsigned long tiempoInicioEco     = 0;
static unsigned long tiempoInicioEspera  = 0;

// ====================================================
//  FUNCIÓN PRINCIPAL: Máquina de estados no bloqueante
// ====================================================
void MedicionUltrasonico() {
  unsigned long ahora = micros();

  switch (estadoActual) {

    case US_INICIAR_TRIGGER:
      // Inicia pulso de disparo (nivel alto)
      TRIG_ES_ALTO;
      tiempoInicioTrigger = ahora;
      estadoActual = US_FINALIZAR_TRIGGER;
      break;

    case US_FINALIZAR_TRIGGER:
      // Pulso de 10 microsegundos en TRIG
      if (ahora - tiempoInicioTrigger >= 10) {
        TRIG_ES_BAJO;
        tiempoInicioEspera = ahora;
        estadoActual = US_ESPERAR_ECHO_ALTO;
      }
      break;

    case US_ESPERAR_ECHO_ALTO:
      // Espera el inicio del eco (flanco ascendente)
      if (ECHO_ES_ALTO) {
        tiempoInicioEco = ahora;
        estadoActual = US_ESPERAR_ECHO_BAJO;
      } 
      // Timeout de 30 ms si no hay eco
      else if (ahora - tiempoInicioEspera > 30000) { 
        distanciaCm = -1; // sin lectura válida
        estadoActual = US_INICIAR_TRIGGER;
      }
      break;

    case US_ESPERAR_ECHO_BAJO:
      // Cuando el pin ECHO baja, se termina el pulso
      if (ECHO_ES_BAJO) {
        long pulso = ahora - tiempoInicioEco;
        distanciaCm = pulso * 0.0343f / 2.0f;  // cálculo en cm
        // pulsoMicrosegundos puede guardarse si se necesita
        pulsoMicrosegundos = pulso;
        // Vuelve al estado inicial para la próxima medición
        estadoActual = US_INICIAR_TRIGGER;
      }
      break;
  }
}

// ==================================
//  Envolturas públicas del módulo
// ==================================
void ProcesarSensorUltrasonico() {
  // Llamar periódicamente desde loop()
  MedicionUltrasonico();
}

long DistanciaSensorUltrasonico() {
  // Devuelve la última distancia medida (o -1 si falló)
  return distanciaCm;
}

unsigned long ObtenerPulsoUltrasonicoUS() {
  // Devuelve la duración del último pulso en microsegundos
  return pulsoMicrosegundos;
}
