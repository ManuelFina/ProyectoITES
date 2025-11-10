#include "CtrlServo.h"
#include <Servo.h>

Servo servoBarrido;

int anguloActual = 0;
int direccionPaso = 1;
unsigned long tiempoPrevioMovimiento = 0;
const unsigned intervaloMovimiento = 20;

void MovimientoServo() {
  unsigned long ahora = millis();

  if (ahora - tiempoPrevioMovimiento >= intervaloMovimiento) {
    tiempoPrevioMovimiento = ahora;

    anguloActual += direccionPaso;

    if (anguloActual >= 180) direccionPaso = -1;
    if (anguloActual <= 0)   direccionPaso = 1;

    servoBarrido.write(anguloActual);
  }
}

int ObtenerAnguloServo() {
  return anguloActual;
}
