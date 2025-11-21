#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include "htmlControl.h"

#define WIFI_SSID   "Alumnos"
#define WIFI_PASS   ""

#define MQTT_BROKER      "192.168.0.4"
#define MQTT_PORT        1883
#define MQTT_CLIENT_ID   "esp32_tanque_01"
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

#define CONFIG_PWM       { ledcAttach(PIN_ENA, PWM_FREQ, PWM_BITS); ledcAttach(PIN_ENB, PWM_FREQ, PWM_BITS); }
#define VELOCIDAD_MAXIMA { ledcWrite(PIN_ENA, DUTY_MAX); ledcWrite(PIN_ENB, DUTY_MAX); }
#define DISTANCIA_MINIMA 20   // cm

#define CONFIG_MOTORES  { pinMode(PIN_MOTOR1_A, OUTPUT); pinMode(PIN_MOTOR1_B, OUTPUT); pinMode(PIN_MOTOR2_A, OUTPUT); pinMode(PIN_MOTOR2_B, OUTPUT); }

#define AVANZAR         { digitalWrite(PIN_MOTOR1_A, HIGH); digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, HIGH); digitalWrite(PIN_MOTOR2_B, LOW); }
#define RETROCEDER      { digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, HIGH); digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, HIGH); }
#define ROTAR_DERECHA   { digitalWrite(PIN_MOTOR1_A, HIGH); digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, HIGH); }
#define ROTAR_IZQUIERDA { digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, HIGH); digitalWrite(PIN_MOTOR2_A, HIGH); digitalWrite(PIN_MOTOR2_B, LOW); }
#define DETENER_MOTORES { digitalWrite(PIN_MOTOR1_A, LOW);  digitalWrite(PIN_MOTOR1_B, LOW);  digitalWrite(PIN_MOTOR2_A, LOW);  digitalWrite(PIN_MOTOR2_B, LOW); }

#define PIN_SERVO       23
#define CONFIG_SERVO    { servoBarrido.attach(PIN_SERVO); servoBarrido.write(anguloActual); }

#define PIN_TRIG        12
#define PIN_ECHO        13

#define CONFIG_ULTRASONICO  { pinMode(PIN_TRIG, OUTPUT); pinMode(PIN_ECHO, INPUT); TRIG_ES_BAJO; }

#define TRIG_ES_ALTO    digitalWrite(PIN_TRIG, HIGH)
#define TRIG_ES_BAJO    digitalWrite(PIN_TRIG, LOW)

#define ECHO_ES_ALTO    (digitalRead(PIN_ECHO) == HIGH)
#define ECHO_ES_BAJO    (digitalRead(PIN_ECHO) == LOW)

#define PIN_LED         2
#define CONFIG_LED      pinMode(PIN_LED, OUTPUT);
#define MS_ESPERA_LED   500
#define ENCENDER_LED    digitalWrite(PIN_LED, HIGH)
#define APAGAR_LED      digitalWrite(PIN_LED, LOW)

enum Movimiento { PARADO, ADELANTE, ATRAS, IZQ, DER };

WiFiClient   clienteWiFi;
PubSubClient brokerMQTT(clienteWiFi);
WebServer    servidorHTTP(80);
Servo        servoBarrido;

long         distanciaCm   = -1;
int          anguloActual  = 90;
volatile Movimiento ordenMovimiento = PARADO;

enum EstadoUltrasonico {
  US_INICIAR_TRIGGER,
  US_FINALIZAR_TRIGGER,
  US_ESPERAR_ECHO_ALTO,
  US_ESPERAR_ECHO_BAJO
};

void Blink() {
  static unsigned long millis_ini = 0;
  static bool estado_led = false;

  if ((millis() - millis_ini) < MS_ESPERA_LED) return;
  millis_ini = millis();

  estado_led = !estado_led;

  if (estado_led) ENCENDER_LED;
  else            APAGAR_LED;
}

void ConectarWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Conectando a WiFi...");
}

void ReconectarWiFi() {
  static unsigned long millis_ini = 0;
  const unsigned long INTERVALO = 1000;  

  if (WiFi.status() == WL_CONNECTED) return;

  if ((millis() - millis_ini) < INTERVALO) return;
  millis_ini = millis();

  WiFi.reconnect();
  Serial.print(".");
}


void ConfigurarClienteMQTT() {
  brokerMQTT.setServer(MQTT_BROKER, MQTT_PORT);
}

void ReconectarMQTT() {
  static unsigned long millis_ini = 0;
  const unsigned long INTERVALO_REINTENTO_MQTT = 2000;

  if (brokerMQTT.connected()) return;

  if (millis() - millis_ini < INTERVALO_REINTENTO_MQTT) return;
  millis_ini = millis();

  if   (brokerMQTT.connect(MQTT_CLIENT_ID)) Serial.println("MQTT conectado");
  else Serial.println("MQTT fallo al conectar");
  
}

void ActualizarClienteMQTT() {
  ReconectarMQTT();
  if (brokerMQTT.connected()) brokerMQTT.loop();
}

void EnviarMedicionMQTT() {
  static unsigned long millis_ini = 0;
  const unsigned long INTERVALO_PUBLISH_MS = 250;

  if (!brokerMQTT.connected()) return;

  if (millis() - millis_ini < INTERVALO_PUBLISH_MS) return;
  millis_ini = millis();

  String payload = "{";
  payload += "\"device_id\":\"" + String(MQTT_CLIENT_ID) + "\"";
  payload += ",\"distance_cm\":" + String(distanciaCm, 2);
  payload += ",\"angle_deg\":" + String(anguloActual);
  payload += ",\"ts\":" + String(millis_ini);
  payload += "}";

  brokerMQTT.publish(MQTT_TOPIC_MEDICIONES, payload.c_str());
}

void MedicionUltrasonico() {
  static EstadoUltrasonico estadoUS        = US_INICIAR_TRIGGER;
  static unsigned long tiempoInicioTrigger = 0;
  static unsigned long tiempoInicioEco     = 0;
  static unsigned long tiempoInicioEspera  = 0;
  static unsigned long pulsoMicrosegundos  = 0;

  unsigned long ahora = micros();

  switch (estadoUS) {
    case US_INICIAR_TRIGGER:
      TRIG_ES_ALTO;
      tiempoInicioTrigger = ahora;
      estadoUS = US_FINALIZAR_TRIGGER;
      break;

    case US_FINALIZAR_TRIGGER:
      if (ahora - tiempoInicioTrigger < 10) return;

      TRIG_ES_BAJO;
      tiempoInicioEspera = ahora;
      estadoUS = US_ESPERAR_ECHO_ALTO;
      break;

    case US_ESPERAR_ECHO_ALTO:
      if (ahora - tiempoInicioEspera > 30000) {
        distanciaCm = -1;
        estadoUS = US_INICIAR_TRIGGER;
        break;
      }

      if (!ECHO_ES_ALTO) break;

      tiempoInicioEco = ahora;
      estadoUS = US_ESPERAR_ECHO_BAJO;
      break;

    case US_ESPERAR_ECHO_BAJO:
      if (!ECHO_ES_BAJO) break;

      {
        unsigned long pulso = ahora - tiempoInicioEco;
        pulsoMicrosegundos  = pulso;
        distanciaCm         = pulso * 0.0343f / 2.0f;
        estadoUS            = US_INICIAR_TRIGGER;
      }
      break;
  }
}

void MovimientoTanque() {
  const long distancia = distanciaCm;

  if (distancia >= 0 && distancia < DISTANCIA_MINIMA) ordenMovimiento = PARADO;
  
}

void CtrlOrugas() {
  static Movimiento ultimoOrden = PARADO;

  if (ordenMovimiento == ultimoOrden) return;

  ultimoOrden = ordenMovimiento;

  switch (ordenMovimiento) {
    case ADELANTE:    AVANZAR;         break;
    case ATRAS:       RETROCEDER;      break;
    case IZQ:         ROTAR_IZQUIERDA; break;
    case DER:         ROTAR_DERECHA;   break;
    case PARADO:
    default:          DETENER_MOTORES; break;
  }
}

void MovimientoServo() {
  static int paso = 1;
  static unsigned long millis_ini = 0;
  const unsigned long INTERVALO = 20;

  if ( (millis() - millis_ini) < INTERVALO) return;
  millis_ini = millis();

  anguloActual += paso;

  if (anguloActual >= 180) paso = -1;
  if (anguloActual <= 0)   paso = 1;

  servoBarrido.write(anguloActual);
}

void ConfigurarServidorWeb() {
  servidorHTTP.on("/", []() {
    servidorHTTP.send(200, "text/html", generarHTML());
  });

  servidorHTTP.on("/adelante", []() {
    ordenMovimiento = ADELANTE;
    servidorHTTP.send(200, "text/plain", "OK");
  });
  servidorHTTP.on("/atras", []() {
    ordenMovimiento = ATRAS;
    servidorHTTP.send(200, "text/plain", "OK");
  });
  servidorHTTP.on("/izquierda", []() {
    ordenMovimiento = IZQ;
    servidorHTTP.send(200, "text/plain", "OK");
  });
  servidorHTTP.on("/derecha", []() {
    ordenMovimiento = DER;
    servidorHTTP.send(200, "text/plain", "OK");
  });
  servidorHTTP.on("/parar", []() {
    ordenMovimiento = PARADO;
    servidorHTTP.send(200, "text/plain", "OK");
  });

  servidorHTTP.on("/ping", []() {
    servidorHTTP.send(200, "text/plain", "pong");
  });

  servidorHTTP.begin();
  Serial.println("[HTTP] Servidor iniciado");
}

void ActualizarServidorWeb() {
  servidorHTTP.handleClient();
}

void setup() {
  Serial.begin(115200);

  CONFIG_LED
  CONFIG_MOTORES
  CONFIG_SERVO
  CONFIG_ULTRASONICO
  CONFIG_PWM
  VELOCIDAD_MAXIMA
  DETENER_MOTORES

  ConectarWiFi();
  ConfigurarServidorWeb();
  ConfigurarClienteMQTT();
}

void loop() {
  ActualizarServidorWeb();
  ActualizarClienteMQTT();
  ReconectarWiFi();

  MedicionUltrasonico();
  MovimientoTanque();
  CtrlOrugas();
  MovimientoServo();

  Blink();

  if (distanciaCm >= 0) EnviarMedicionMQTT();
}
