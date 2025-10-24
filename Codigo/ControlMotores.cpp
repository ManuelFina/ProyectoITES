#include "ControlMotores.h"

ControlMotores motores;

void ControlMotores::configurarPines() {
  CONFIGURAR_PIN_MOTOR1_A();
  CONFIGURAR_PIN_MOTOR1_B();
  CONFIGURAR_PIN_MOTOR2_A();
  CONFIGURAR_PIN_MOTOR2_B();
}

void ControlMotores::adelante() {
  MOTOR_HIGH(PIN_MOTOR1_A);
  MOTOR_LOW(PIN_MOTOR1_B);
  MOTOR_HIGH(PIN_MOTOR2_A);
  MOTOR_LOW(PIN_MOTOR2_B);
  PWM_WRITE(CANAL_PWM1, velocidadBase);
  PWM_WRITE(CANAL_PWM2, velocidadBase);
}

void ControlMotores::atras() {
  MOTOR_LOW(PIN_MOTOR1_A);
  MOTOR_HIGH(PIN_MOTOR1_B);
  MOTOR_LOW(PIN_MOTOR2_A);
  MOTOR_HIGH(PIN_MOTOR2_B);
  PWM_WRITE(CANAL_PWM1, velocidadBase);
  PWM_WRITE(CANAL_PWM2, velocidadBase);
}

void ControlMotores::izquierda() {
  MOTOR_LOW(PIN_MOTOR1_A);
  MOTOR_HIGH(PIN_MOTOR1_B);
  MOTOR_HIGH(PIN_MOTOR2_A);
  MOTOR_LOW(PIN_MOTOR2_B);
  PWM_WRITE(CANAL_PWM1, velocidadBase);
  PWM_WRITE(CANAL_PWM2, velocidadBase);
}

void ControlMotores::derecha() {
  MOTOR_HIGH(PIN_MOTOR1_A);
  MOTOR_LOW(PIN_MOTOR1_B);
  MOTOR_LOW(PIN_MOTOR2_A);
  MOTOR_HIGH(PIN_MOTOR2_B);
  PWM_WRITE(CANAL_PWM1, velocidadBase);
  PWM_WRITE(CANAL_PWM2, velocidadBase);
}

void ControlMotores::detener() {
  MOTOR_LOW(PIN_MOTOR1_A);
  MOTOR_LOW(PIN_MOTOR1_B);
  MOTOR_LOW(PIN_MOTOR2_A);
  MOTOR_LOW(PIN_MOTOR2_B);
  PWM_WRITE(CANAL_PWM1, 0);
  PWM_WRITE(CANAL_PWM2, 0);
}
