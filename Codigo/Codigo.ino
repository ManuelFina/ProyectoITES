#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

#define WIFI_SSID             "CPE-FOKUWI"
#define WIFI_PASS             "001dd6408e22"

#define MQTT_BROKER           "192.168.1.100"
#define MQTT_PORT             1883
#define MQTT_CLIENT_ID        "esp32_tanque_01"
#define MQTT_TOPIC_TELEMETRIA "/tank/esp32_01/telemetry"
#define MQTT_TOPIC_EVENTO     "/tank/esp32_01/event"
#define MQTT_TOPIC_COMANDO    "/tank/command"

#define PIN_MOTOR1_A 16
#define PIN_MOTOR1_B 17
#define PIN_MOTOR2_A 18
#define PIN_MOTOR2_B 19
#define PIN_PWM1     25
#define PIN_PWM2     26
#define PIN_TRIGGER  12
#define PIN_ECHO     13
#define PIN_VBAT     35
#define PIN_SERVO    14

#define PWM_FREQ     2000
#define PWM_RES      8
#define CANAL_PWM1   0
#define CANAL_PWM2   1

#define SERVO_ANGULO_CENTRO    90
#define SERVO_ANGULO_IZQUIERDA 0
#define SERVO_ANGULO_DERECHA   180

#define MOTOR1_ADELANTE digitalWrite(PIN_MOTOR1_A, HIGH); digitalWrite(PIN_MOTOR1_B, LOW)
#define MOTOR1_ATRAS    digitalWrite(PIN_MOTOR1_A, LOW); digitalWrite(PIN_MOTOR1_B, HIGH)
#define MOTOR1_DETENER  digitalWrite(PIN_MOTOR1_A, LOW); digitalWrite(PIN_MOTOR1_B, LOW)

#define MOTOR2_ADELANTE digitalWrite(PIN_MOTOR2_A, HIGH); digitalWrite(PIN_MOTOR2_B, LOW)
#define MOTOR2_ATRAS    digitalWrite(PIN_MOTOR2_A, LOW); digitalWrite(PIN_MOTOR2_B, HIGH)
#define MOTOR2_DETENER  digitalWrite(PIN_MOTOR2_A, LOW); digitalWrite(PIN_MOTOR2_B, LOW)

#define PWM_AMBOS(v)    ledcWrite(CANAL_PWM1, (v)); ledcWrite(CANAL_PWM2, (v))
#define PWM_M1(v)       ledcWrite(CANAL_PWM1, (v))
#define PWM_M2(v)       ledcWrite(CANAL_PWM2, (v))

#define US_ACTIVAR      digitalWrite(PIN_TRIGGER, HIGH)
#define US_RESETEAR     digitalWrite(PIN_TRIGGER, LOW)
#define US_LEER         digitalRead(PIN_ECHO)

#define VBAT_LEER         analogRead(PIN_VBAT)
#define VBAT_A_VOLTIOS(x) ((x / 4095.0f) * 3.3f * 2.0f)

#define INTERVALO_TELEMETRIA_MS 2000UL
#define INTERVALO_US_MS         100UL
#define DURACION_TRIGGER_US     10UL

#define CONFIG_PIN_MOTOR1_A  pinMode(PIN_MOTOR1_A, OUTPUT)
#define CONFIG_PIN_MOTOR1_B  pinMode(PIN_MOTOR1_B, OUTPUT)
#define CONFIG_PIN_MOTOR2_A  pinMode(PIN_MOTOR2_A, OUTPUT)
#define CONFIG_PIN_MOTOR2_B  pinMode(PIN_MOTOR2_B, OUTPUT)
#define CONFIG_PIN_TRIGGER   pinMode(PIN_TRIGGER, OUTPUT)
#define CONFIG_PIN_ECHO      pinMode(PIN_ECHO, INPUT)
#define CONFIG_PIN_VBAT      pinMode(PIN_VBAT, INPUT)

WiFiClient   clienteWifi;
PubSubClient clienteMqtt(clienteWifi);
WebServer    servidor(80);
Servo        servoTorreta;

int velocidadBase = 200;
unsigned long ultimoEnvioTelemetria = 0;

enum EstadoUS { 
  US_INACTIVO, 
  US_TRIGGER, 
  US_ESPERANDO_ALTO, 
  US_MIDIENDO, 
  US_ESPERANDO_BAJO 
};

EstadoUS estadoUS = US_INACTIVO;
unsigned long ultimoDisparoUS_ms = 0;
unsigned long tiempoInicioTrigger_us = 0;
unsigned long tiempoInicioEcho_us = 0;
unsigned long tiempoFinEcho_us = 0;
long distanciaCm = -1;

enum Comando {
  CMD_ADELANTE,
  CMD_ATRAS,
  CMD_IZQUIERDA,
  CMD_DERECHA,
  CMD_PARAR,
  CMD_SERVO_CENTRO,
  CMD_SERVO_IZQUIERDA,
  CMD_SERVO_DERECHA,
  CMD_DESCONOCIDO
};

String  generarHTML();
void    publicarEvento(const char* accion);
void    moverAdelante();
void    moverAtras();
void    girarIzquierda();
void    girarDerecha();
void    detenerTodo();
void    servoAlCentro();
void    servoAIzquierda();
void    servoADerecha();
void    enviarTelemetria();
void    procesarUS();
Comando parsearComando(const String& mensaje);
void    ejecutarComando(Comando cmd);
void    callbackMqtt(char* topic, byte* payload, unsigned int longitud);
void    reconectarMqtt();
void    configurarPines();
void    configurarPWM();
void    configurarServo();
void    conectarWiFi();
void    configurarMQTT();
void    configurarServidor();
void    actualizarConexiones();
void    actualizarTelemetria();

void configurarPines() {
  CONFIG_PIN_MOTOR1_A;
  CONFIG_PIN_MOTOR1_B;
  CONFIG_PIN_MOTOR2_A;
  CONFIG_PIN_MOTOR2_B;
  CONFIG_PIN_TRIGGER;
  CONFIG_PIN_ECHO;
  CONFIG_PIN_VBAT;
  US_RESETEAR;
}

String generarHTML() {
  return
  "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
  "<style>body{font-family:Arial;background:#0b1220;color:#fff;text-align:center;padding:10px}"
  "button{width:100px;height:80px;margin:6px;font-size:24px;border-radius:8px}"
  ".servo-btn{width:80px;height:60px}</style>"
  "</head><body><h2>Control Tanque</h2>"
  "<div><button onclick=\"fetch('/adelante')\">▲</button></div>"
  "<div><button onclick=\"fetch('/izquierda')\">◀</button>"
  "<button onclick=\"fetch('/parar')\">■</button>"
  "<button onclick=\"fetch('/derecha')\">▶</button></div>"
  "<div><button onclick=\"fetch('/atras')\">▼</button></div>"
  "<h3>Torreta</h3>"
  "<div><button class='servo-btn' onclick=\"fetch('/servo/izquierda')\">◀</button>"
  "<button class='servo-btn' onclick=\"fetch('/servo/centro')\">⬤</button>"
  "<button class='servo-btn' onclick=\"fetch('/servo/derecha')\">▶</button></div>"
  "</body></html>";
}

void publicarEvento(const char* accion) {
  if (!clienteMqtt.connected()) return;
  
  String payload = String("{\"accion\":\"") + accion + "\",\"ts\":" + String(millis()) + "}";
  clienteMqtt.publish(MQTT_TOPIC_EVENTO, payload.c_str());
}

void moverAdelante() {
  MOTOR1_ADELANTE; 
  MOTOR2_ADELANTE; 
  PWM_AMBOS(velocidadBase);
  publicarEvento("adelante");
}

void moverAtras() {
  MOTOR1_ATRAS; 
  MOTOR2_ATRAS; 
  PWM_AMBOS(velocidadBase);
  publicarEvento("atras");
}

void girarIzquierda() {
  MOTOR1_ATRAS; 
  MOTOR2_ADELANTE; 
  PWM_AMBOS(velocidadBase);
  publicarEvento("izquierda");
}

void girarDerecha() {
  MOTOR1_ADELANTE; 
  MOTOR2_ATRAS; 
  PWM_AMBOS(velocidadBase);
  publicarEvento("derecha");
}

void detenerTodo() {
  MOTOR1_DETENER; 
  MOTOR2_DETENER; 
  PWM_AMBOS(0);
  publicarEvento("parar");
}

void servoAlCentro() {
  servoTorreta.write(SERVO_ANGULO_CENTRO);
  publicarEvento("servo_centro");
}

void servoAIzquierda() {
  servoTorreta.write(SERVO_ANGULO_IZQUIERDA);
  publicarEvento("servo_izquierda");
}

void servoADerecha() {
  servoTorreta.write(SERVO_ANGULO_DERECHA);
  publicarEvento("servo_derecha");
}

void enviarTelemetria() {
  if (!clienteMqtt.connected()) return;
  
  int   raw = VBAT_LEER;
  float voltaje = VBAT_A_VOLTIOS(raw);

  String payload = "{";
  payload += "\"distancia\":" + String(distanciaCm);
  payload += ",\"bateria\":" + String(voltaje, 2);
  payload += ",\"servo\":" + String(servoTorreta.read());
  payload += ",\"ts\":" + String(millis());
  payload += "}";

  clienteMqtt.publish(MQTT_TOPIC_TELEMETRIA, payload.c_str());
}

void procesarUS() {
  unsigned long ahora_ms = millis();
  unsigned long ahora_us = micros();

  switch (estadoUS) {
    case US_INACTIVO:
      if (ahora_ms - ultimoDisparoUS_ms < INTERVALO_US_MS) break;
      
      US_ACTIVAR;
      tiempoInicioTrigger_us = ahora_us;
      estadoUS = US_TRIGGER;
      break;

    case US_TRIGGER:
      if (ahora_us - tiempoInicioTrigger_us < DURACION_TRIGGER_US) break;
      
      US_RESETEAR;
      estadoUS = US_ESPERANDO_ALTO;
      break;

    case US_ESPERANDO_ALTO:
      if (US_LEER != HIGH) break;
      
      tiempoInicioEcho_us = ahora_us;
      estadoUS = US_MIDIENDO;
      break;

    case US_MIDIENDO:
      if (US_LEER != LOW) break;
      
      tiempoFinEcho_us = ahora_us;
      unsigned long duracion = tiempoFinEcho_us - tiempoInicioEcho_us;
      distanciaCm = (long)(duracion * 0.0343f / 2.0f);
      ultimoDisparoUS_ms = ahora_ms;
      estadoUS = US_INACTIVO;
      break;

    case US_ESPERANDO_BAJO:
      estadoUS = US_INACTIVO;
      break;
  }
}

Comando parsearComando(const String& mensaje) {
  String m = mensaje; 
  m.toLowerCase();
  
  if (m.indexOf("forward") >= 0 || m.indexOf("adelante") >= 0) return CMD_ADELANTE;
  if (m.indexOf("back") >= 0 || m.indexOf("atrás") >= 0 || m.indexOf("atras") >= 0) return CMD_ATRAS;
  if (m.indexOf("left") >= 0 || m.indexOf("izquierda") >= 0) return CMD_IZQUIERDA;
  if (m.indexOf("right") >= 0 || m.indexOf("derecha") >= 0) return CMD_DERECHA;
  if (m.indexOf("stop") >= 0 || m.indexOf("parar") >= 0) return CMD_PARAR;
  if (m.indexOf("servo_center") >= 0 || m.indexOf("servo_centro") >= 0) return CMD_SERVO_CENTRO;
  if (m.indexOf("servo_left") >= 0 || m.indexOf("servo_izquierda") >= 0) return CMD_SERVO_IZQUIERDA;
  if (m.indexOf("servo_right") >= 0 || m.indexOf("servo_derecha") >= 0) return CMD_SERVO_DERECHA;
  
  return CMD_DESCONOCIDO;
}

void ejecutarComando(Comando cmd) {
  switch (cmd) {
    case CMD_ADELANTE:        moverAdelante();    break;
    case CMD_ATRAS:           moverAtras();       break;
    case CMD_IZQUIERDA:       girarIzquierda();   break;
    case CMD_DERECHA:         girarDerecha();     break;
    case CMD_PARAR:           detenerTodo();      break;
    case CMD_SERVO_CENTRO:    servoAlCentro();    break;
    case CMD_SERVO_IZQUIERDA: servoAIzquierda();  break;
    case CMD_SERVO_DERECHA:   servoADerecha();    break;
    default: break;
  }
}

void callbackMqtt(char* topic, byte* payload, unsigned int longitud) {
  String mensaje;
  mensaje.reserve(longitud);
  for (unsigned int i = 0; i < longitud; i++) mensaje += (char)payload[i];
  ejecutarComando(parsearComando(mensaje));
}

void reconectarMqtt() {
  while (!clienteMqtt.connected()) {
    if (clienteMqtt.connect(MQTT_CLIENT_ID)) {
      clienteMqtt.subscribe(MQTT_TOPIC_COMANDO);
      break;
    }
    
    clienteMqtt.loop();
    yield();
  }
}

void configurarPWM() {
  ledcSetup(CANAL_PWM1, PWM_FREQ, PWM_RES);
  ledcSetup(CANAL_PWM2, PWM_FREQ, PWM_RES);
  ledcAttachPin(PIN_PWM1, CANAL_PWM1);
  ledcAttachPin(PIN_PWM2, CANAL_PWM2);
}

void configurarServo() {
  servoTorreta.attach(PIN_SERVO);
  servoAlCentro();
}

void conectarWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    yield();
  }
}

void configurarMQTT() {
  clienteMqtt.setServer(MQTT_BROKER, MQTT_PORT);
  clienteMqtt.setCallback(callbackMqtt);
  reconectarMqtt();
}

void configurarServidor() {
  servidor.on("/", [](){ 
    servidor.send(200, "text/html", generarHTML()); 
  });

  servidor.on("/adelante", [](){ 
    moverAdelante(); 
    servidor.send(200, "text/plain", "OK"); 
  });
  
  servidor.on("/atras", [](){ 
    moverAtras(); 
    servidor.send(200, "text/plain", "OK"); 
  });
  
  servidor.on("/izquierda", [](){ 
    girarIzquierda(); 
    servidor.send(200, "text/plain", "OK"); 
  });
  
  servidor.on("/derecha", [](){ 
    girarDerecha(); 
    servidor.send(200, "text/plain", "OK"); 
  });
  
  servidor.on("/parar", [](){ 
    detenerTodo(); 
    servidor.send(200, "text/plain", "OK"); 
  });

  servidor.on("/servo/centro", [](){ 
    servoAlCentro(); 
    servidor.send(200, "text/plain", "OK"); 
  });

  servidor.on("/servo/izquierda", [](){ 
    servoAIzquierda(); 
    servidor.send(200, "text/plain", "OK"); 
  });

  servidor.on("/servo/derecha", [](){ 
    servoADerecha(); 
    servidor.send(200, "text/plain", "OK"); 
  });

  servidor.begin();
}

void actualizarConexiones() {
  if (!clienteMqtt.connected()) reconectarMqtt();
  
  clienteMqtt.loop();
  servidor.handleClient();
}

void actualizarTelemetria() {
  unsigned long ahora = millis();
  
  if (ahora - ultimoEnvioTelemetria < INTERVALO_TELEMETRIA_MS) return;
  
  enviarTelemetria();
  ultimoEnvioTelemetria = ahora;
}

void setup() {
  configurarPines();
  configurarPWM();
  configurarServo();
  detenerTodo();
  conectarWiFi();
  configurarMQTT();
  configurarServidor();
  ultimoEnvioTelemetria = millis();
}

void loop() {
  actualizarConexiones();
  procesarUS();
  actualizarTelemetria();
}