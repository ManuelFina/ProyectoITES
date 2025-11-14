#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>  

#define WIFI_SSID   "Tanque-123"
#define WIFI_PASS   "tanque12345"
#define MQTT_BROKER "192.168.50.10"
#define MQTT_PORT    1883
#define MQTT_CLIENT_ID "esp32_tanque_01"
#define MQTT_TOPIC_MEDICIONES "/tank/esp32_01/measurements"

#define PIN_MOTOR1_A 25
#define PIN_MOTOR1_B 26
#define PIN_MOTOR2_A 27
#define PIN_MOTOR2_B 14
#define PIN_ENA      32
#define PIN_ENB      33

#define PWM_FREQ 2000
#define PWM_BITS 8
#define DUTY_MAX 255

#define CONFIGURAR_PWM   {ledcAttach(PIN_ENA, PWM_FREQ, PWM_BITS); ledcAttach(PIN_ENB, PWM_FREQ, PWM_BITS);}
#define VELOCIDAD_MAXIMA {ledcWrite(PIN_ENA, DUTY_MAX); ledcWrite(PIN_ENB, DUTY_MAX);}
#define DISTANCIA_MINIMA 20

#define CONFIG_MOTORES  {pinMode(PIN_MOTOR1_A, OUTPUT); pinMode(PIN_MOTOR1_B, OUTPUT); pinMode(PIN_MOTOR2_A, OUTPUT); pinMode(PIN_MOTOR2_B, OUTPUT);}

#define AVANZAR         {digitalWrite(PIN_MOTOR1_A, HIGH); digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, HIGH); digitalWrite(PIN_MOTOR2_B, LOW);}
#define RETROCEDER      {digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, HIGH); digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, HIGH);}
#define ROTAR_DERECHA   {digitalWrite(PIN_MOTOR1_A, HIGH); digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, HIGH);}
#define ROTAR_IZQUIERDA {digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, HIGH); digitalWrite(PIN_MOTOR2_A, HIGH); digitalWrite(PIN_MOTOR2_B, LOW);}
#define DETENER_MOTORES {digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, LOW);}

#define PIN_SERVO      23
#define CONFIG_SERVO   {servoBarrido.attach(PIN_SERVO); servoBarrido.write(anguloActual);}

#define PIN_TRIG       12
#define PIN_ECHO       13
#define TIEMPO_ESCANEO 20

#define CONFIG_ULTRASONICO  {pinMode(PIN_TRIG, OUTPUT); pinMode(PIN_ECHO, INPUT);}

#define TRIG_ES_ALTO   digitalWrite(PIN_TRIG, HIGH)
#define TRIG_ES_BAJO   digitalWrite(PIN_TRIG, LOW)

#define ECHO_ES_ALTO    (digitalRead(PIN_ECHO) == HIGH)
#define ECHO_ES_BAJO    (digitalRead(PIN_ECHO) == LOW)

#endif
