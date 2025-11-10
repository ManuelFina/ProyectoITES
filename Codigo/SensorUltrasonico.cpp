#include "SensorUltrasonico.h"
#include "Config.h"

#include "SensorUltrasonico.h"
#include "Config.h"
#include "CtrlServo.h"
#include "ClienteMQTT.h"

float distanciaCm = 0;
long pulsoMicrosegundos = 0;
float anguloMedido = 0;

enum EstadoUltrasonico {
  US_INICIAR_TRIGGER,      
  US_FINALIZAR_TRIGGER,    
  US_ESPERAR_ECHO_ALTO,   
  US_ESPERAR_ECHO_BAJO     
};

EstadoUltrasonico estadoActual = US_INICIAR_TRIGGER;

unsigned long tiempoInicioTrigger = 0;
unsigned long tiempoInicioEco =     0;
unsigned long tiempoInicioEspera =  0;

void MedicionUltrasonico() {
  unsigned long ahora = micros();

  switch (estadoActual) {

    // 1) Subir TRIG (inicio del pulso)
    case US_INICIAR_TRIGGER:
      digitalWrite(PIN_TRIG, HIGH);
      tiempoInicioTrigger = ahora;
      estadoActual = US_FINALIZAR_TRIGGER;
      break;

    // 2) Bajar TRIG cuando pasen 10 μs
    case US_FINALIZAR_TRIGGER:
      if (ahora - tiempoInicioTrigger >= 10) {
        digitalWrite(PIN_TRIG, LOW);
        tiempoInicioEspera = ahora;
        estadoActual = US_ESPERAR_ECHO_ALTO;
      }
      break;

    // 3) Esperar que ECHO suba (eco recibido)
    case US_ESPERAR_ECHO_ALTO:
      if (digitalRead(PIN_ECHO) == HIGH) {
        tiempoInicioEco = ahora;
        estadoActual = US_ESPERAR_ECHO_BAJO;
      }

      // timeout sin eco
      if (ahora - tiempoInicioEspera > 30000) {
        distanciaCm = -1;
        estadoActual = US_INICIAR_TRIGGER;
      }
      break;

    // 4) Esperar que ECHO baje (eco termina)
    case US_ESPERAR_ECHO_BAJO:
      if (digitalRead(PIN_ECHO) == LOW) {
        pulsoMicrosegundos = ahora - tiempoInicioEco;
        distanciaCm = pulsoMicrosegundos * 0.0343 / 2;
        anguloMedido = ObtenerAnguloServo();

        EnviarMedicionMQTT();

        estadoActual = US_INICIAR_TRIGGER;
      }
      break;
  }
}

