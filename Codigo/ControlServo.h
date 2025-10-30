/* todo esto tiene que estar en macros 

Servo miServo;

int angulo = 0;
int paso = 1; // +1 sube, -1 baja

unsigned long tiempoAnterior = 0;
const unsigned long intervalo = 20; // velocidad del movimiento (ms)

void setup() {
  miServo.attach(3); // Servo en pin 3
  miServo.write(angulo);
}
*/
void MovimientoServo() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;

    angulo += paso;
//refactorizar esto
    if (angulo >= 180) {
      paso = -1;
    }
    if (angulo <= 0) {
      paso = 1;
    }

    miServo.write(angulo);
  }

}