#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <Servo.h>

#define WIFI_SSID "Tanque_WIFI" 
#define WIFI_PASS "tanque12345" 
#define MQTT_BROKER "192.168.50.10" 
#define MQTT_PORT 1883 
#define MQTT_CLIENT_ID "esp32_tanque_01" 
#define MQTT_TOPIC_MEDICIONES "/tank/esp32_01/measurements"

#define PIN_MOTOR1_A 16
#define PIN_MOTOR1_B 17
#define PIN_MOTOR2_A 18
#define PIN_MOTOR2_B 19

#define CONFIG_MOTORES  {pinMode(PIN_MOTOR1_A, OUTPUT); pinMode(PIN_MOTOR1_B, OUTPUT); pinMode(PIN_MOTOR2_A, OUTPUT); pinMode(PIN_MOTOR2_B, OUTPUT)}

#define AVANZAR         {digitalWrite(PIN_MOTOR1_A, HIGH); digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, HIGH); digitalWrite(PIN_MOTOR2_B, LOW)}
#define RETROCEDER      {digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, HIGH); digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, HIGH)}
#define ROTAR_DERECHA   {digitalWrite(PIN_MOTOR1_A, HIGH); digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, HIGH)}
#define ROTAR_IZQUIERDA {digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, HIGH); digitalWrite(PIN_MOTOR2_A, HIGH); digitalWrite(PIN_MOTOR2_B, LOW)}
#define DETENER_MOTORES {digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, LOW)}

#define PIN_SERVO      9

#define CONFIG_SERVO   {miServo.attach(14); miServo.write(angulo)}

#define PIN_TRIG       7
#define PIN_ECHO       6
#define TIEMPO_ESCANEO 20 

#define CONFIG_ULTRASONICO  {pinMode(PIN_TRIG, OUTPUT); pinMode(PIN_ECHO, INPUT)}

#define TRIG_ON        digitalWrite(PIN_TRIG, HIGH)
#define TRIG_OFF       digitalWrite(PIN_TRIG, LOW)

#define ECO_ES_ALTO    digitalRead(PIN_ECHO) == HIGH)
#define ECO_ES_BAJO    digitalRead(PIN_ECHO) == LOW)


#endif
