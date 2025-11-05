#include MovimientoServo.h
#include <Servo.h>

Servo miServo;

int angulo = 0;
int paso = 1;
unsigned long tiempoAnterior = 0;
const unsigned intervalo = 20;

void MovimientoServo() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;

    angulo += paso;

    if (angulo >= 180)  paso = -1;
    if (angulo <= 0)    paso = 1;

    miServo.write(angulo);
  }

}