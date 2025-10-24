#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <Servo.h>    

#define WIFI_SSID       "iPhone de Enzo"
#define WIFI_PASS       "123456789"

#define MQTT_BROKER     "192.168.1.100"
#define MQTT_PORT       1883
#define MQTT_CLIENT_ID  "esp32_tanque_01"
#define MQTT_TOPIC_MEDICIONES "/tank/esp32_01/measurements"

#define PIN_MOTOR1_A 16
#define PIN_MOTOR1_B 17
#define PIN_MOTOR2_A 18
#define PIN_MOTOR2_B 19
#define PIN_PWM1     25
#define PIN_PWM2     26
#define PIN_TRIGGER  27
#define PIN_ECHO     13
#define PIN_SERVO    14   

#define PIN_ENCENDER(pin)      digitalWrite(pin, HIGH)
#define PIN_APAGAR(pin)        digitalWrite(pin, LOW)
#define PIN_LEER(pin)          digitalRead(pin)

#define CONFIGURAR_PIN_MOTOR1_A()   pinMode(PIN_MOTOR1_A, OUTPUT)
#define CONFIGURAR_PIN_MOTOR1_B()   pinMode(PIN_MOTOR1_B, OUTPUT)
#define CONFIGURAR_PIN_MOTOR2_A()   pinMode(PIN_MOTOR2_A, OUTPUT)
#define CONFIGURAR_PIN_MOTOR2_B()   pinMode(PIN_MOTOR2_B, OUTPUT)

#define PWM_FREQ 2000
#define PWM_RES  8
#define PWM_RESOLUTION (ledc_timer_bit_t)PWM_RES
#define CANAL_PWM1 0
#define CANAL_PWM2 1
#define PWM_SPEED_MODE LEDC_LOW_SPEED_MODE
#define PWM_TIMER LEDC_TIMER_0
#define PWM_CLK_CFG LEDC_AUTO_CLK

#define SERVO_ANGULO_MIN 0
#define SERVO_ANGULO_MAX 180
#define SERVO_PASO 5
#define INTERVALO_SERVO_MS 50

#define INTERVALO_US_MS 100
#define DURACION_TRIGGER_US 10
#define US_VELOCIDAD_SONIDO 0.0343f

#define MOTOR_HIGH(pin) digitalWrite(pin, HIGH)
#define MOTOR_LOW(pin)  digitalWrite(pin, LOW)
#define PWM_WRITE(canal, duty) ledcWrite(canal, duty) 

#endif
