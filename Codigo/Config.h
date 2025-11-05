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

#define ESTADO_PARADO     0
#define ESTADO_AVANZAR    1
#define ESTADO_RETROCEDER 2
#define ESTADO_IZQUIERDA  3
#define ESTADO_DERECHA    4

#define CONFIG_MOTORES  {pinMode(PIN_MOTOR1_A, OUTPUT); pinMode(PIN_MOTOR1_B, OUTPUT); pinMode(PIN_MOTOR2_A, OUTPUT); pinMode(PIN_MOTOR2_B, OUTPUT)}

#define AVANZAR         {digitalWrite(PIN_MOTOR1_A, HIGH); digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, HIGH); digitalWrite(PIN_MOTOR2_B, LOW)}
#define RETROCEDER      {digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, HIGH); digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, HIGH)}
#define ROTAR_DERECHA   {digitalWrite(PIN_MOTOR1_A, HIGH); digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, HIGH)}
#define ROTAR_IZQUIERDA {digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, HIGH); digitalWrite(PIN_MOTOR2_A, HIGH); digitalWrite(PIN_MOTOR2_B, LOW)}
#define DETENER_MOTORES {digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, LOW)}

#define CONFIG_SERVO {miServo.attach(14); miServo.write(angulo)}

#define PIN_TRIGGER  27
#define PIN_ECHO     13

// esto no son funciones osea sacar()
#define CONFIGURAR_PIN_TRIGGER()    pinMode(PIN_TRIGGER, OUTPUT)
#define CONFIGURAR_PIN_ECHO()       pinMode(PIN_ECHO, INPUT)

#define TRIGGER_ACTIVAR()           digitalWrite(PIN_TRIGGER, HIGH)
#define TRIGGER_DESACTIVAR()        digitalWrite(PIN_TRIGGER, LOW)
#define ECHO_ES_ALTO()              (digitalRead(PIN_ECHO) == HIGH)
#define ECHO_ES_BAJO()              (digitalRead(PIN_ECHO) == LOW)

#define TIEMPO_MS()                 millis()
#define TIEMPO_US()                 micros()

#define INTERVALO_US_MS             
#define DURACION_TRIGGER_US        
#define US_VELOCIDAD_SONIDO         

#endif
