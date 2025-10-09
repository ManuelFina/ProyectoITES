#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>

#define WIFI_SSID             "TU_SSID"
#define WIFI_PASS             "TU_PASS"
                              
#define MQTT_BROKER           "192.168.1.100"
#define MQTT_PORT             1883
#define MQTT_CLIENT_ID        "esp32_tanque_01"
#define MQTT_TOPIC_TELEMETRIA "/tank/esp32_01/telemetry"
#define MQTT_TOPIC_EVENTO     "/tank/esp32_01/event"
#define MQTT_TOPIC_COMANDO    "/tank/command"

#define PIN_M1_A 16
#define PIN_M1_B 17
#define PIN_M2_A 18
#define PIN_M2_B 19
#define PIN_PWM1 25
#define PIN_PWM2 26
#define PIN_TRIG 12
#define PIN_ECHO 13
#define PIN_VBAT 35

#define PWM_FREQ 2000
#define PWM_RES 8
#define CANAL_PWM1 0
#define CANAL_PWM2 1

//refactorizar. No son funciones 
#define M1_ADELANTE() digitalWrite(PIN_M1_A, HIGH); digitalWrite(PIN_M1_B, LOW)
#define M1_ATRAS()    digitalWrite(PIN_M1_A, LOW); digitalWrite(PIN_M1_B, HIGH)
#define M1_DETENER()  digitalWrite(PIN_M1_A, LOW); digitalWrite(PIN_M1_B, LOW)

#define M2_ADELANTE() digitalWrite(PIN_M2_A, HIGH); digitalWrite(PIN_M2_B, LOW)
#define M2_ATRAS()    digitalWrite(PIN_M2_A, LOW); digitalWrite(PIN_M2_B, HIGH)
#define M2_DETENER()  digitalWrite(PIN_M2_A, LOW); digitalWrite(PIN_M2_B, LOW)

#define PWM_AMBOS(v)  ledcWrite(CANAL_PWM1, v); ledcWrite(CANAL_PWM2, v)
#define PWM_M1(v)     ledcWrite(CANAL_PWM1, v)
#define PWM_M2(v)     ledcWrite(CANAL_PWM2, v)

#define US_PULSO()    digitalWrite(PIN_TRIG, HIGH)
#define US_RESET()    digitalWrite(PIN_TRIG, LOW)
#define US_LEER()     digitalRead(PIN_ECHO)

#define VBAT_LEER() analogRead(PIN_VBAT)
#define VBAT_A_VOLTIOS(raw) ((raw / 4095.0f) * 3.3f * 2.0f)

#define INTERVALO_TELEMETRIA 2000
#define INTERVALO_ULTRASONIDO 100
#define DURACION_TRIGGER 10

#define INICIALIZAR_PINES() \
  pinMode(PIN_M1_A, OUTPUT); \
  pinMode(PIN_M1_B, OUTPUT); \
  pinMode(PIN_M2_A, OUTPUT); \
  pinMode(PIN_M2_B, OUTPUT); \
  pinMode(PIN_TRIG, OUTPUT); \
  pinMode(PIN_ECHO, INPUT); \
  pinMode(PIN_VBAT, INPUT); \
  US_RESET()

WiFiClient clienteWifi;
PubSubClient clienteMqtt(clienteWifi);
WebServer servidor(80);

int velocidadBase = 200;
unsigned long ultimoEnvioTelemetria = 0;

enum EstadoUS { US_INACTIVO, US_TRIGGER, US_ESPERAR_ECO };
EstadoUS estadoUltrasonido = US_INACTIVO;
unsigned long inicioTriggerMicros = 0;
unsigned long inicioEcoMicros = 0;
long distanciaCm = -1;

String generarPaginaHTML() {
  return
  "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
  "<style>body{font-family:Arial;background:#0b1220;color:#fff;text-align:center;padding:10px}"
  "button{width:100px;height:80px;margin:6px;font-size:24px;border-radius:8px}</style>"
  "</head><body><h2>Control Tanque</h2>"
  "<div><button onclick=\"fetch('/adelante')\">▲</button></div>"
  "<div><button onclick=\"fetch('/izquierda')\">◀</button>"
  "<button onclick=\"fetch('/parar')\">■</button>"
  "<button onclick=\"fetch('/derecha')\">▶</button></div>"
  "<div><button onclick=\"fetch('/atras')\">▼</button></div>"
  "</body></html>";
}

void publicarEvento(const char* accion) {
  if(!clienteMqtt.connected()) return;
  
  String payload = "{\"accion\":\"";
  payload += accion;
  payload += "\",\"ts\":";
  payload += String(millis());
  payload += "}";
  clienteMqtt.publish(MQTT_TOPIC_EVENTO, payload.c_str());
}

void moverAdelante() {
  M1_ADELANTE();
  M2_ADELANTE();
  PWM_AMBOS(velocidadBase);
  publicarEvento("adelante");
}

void moverAtras() {
  M1_ATRAS();
  M2_ATRAS();
  PWM_AMBOS(velocidadBase);
  publicarEvento("atras");
}

void girarIzquierda() {
  M1_ATRAS();
  M2_ADELANTE();
  PWM_AMBOS(velocidadBase);
  publicarEvento("izquierda");
}

void girarDerecha() {
  M1_ADELANTE();
  M2_ATRAS();
  PWM_AMBOS(velocidadBase);
  publicarEvento("derecha");
}

void detenerTodo() {
  M1_DETENER();
  M2_DETENER();
  PWM_AMBOS(0);
  publicarEvento("parar");
}

void enviarTelemetria() {
  if(!clienteMqtt.connected()) return;
  
  int valorRaw = VBAT_LEER();
  float voltaje = VBAT_A_VOLTIOS(valorRaw);
  
  String payload = "{";
  payload += "\"distancia\":";
  payload += String(distanciaCm);
  payload += ",\"bateria\":";
  payload += String(voltaje, 2);
  payload += ",\"ts\":";
  payload += String(millis());
  payload += "}";
  
  clienteMqtt.publish(MQTT_TOPIC_TELEMETRIA, payload.c_str());
}

void procesarUltrasonido() {
  unsigned long ahoraMicros = micros();
  
  switch(estadoUltrasonido) {
    case US_INACTIVO:
      if(millis() - ultimoEnvioTelemetria < INTERVALO_ULTRASONIDO) return;
      
      US_PULSO();
      inicioTriggerMicros = ahoraMicros;
      estadoUltrasonido = US_TRIGGER;
      break;
      
    case US_TRIGGER:
      if(ahoraMicros - inicioTriggerMicros < DURACION_TRIGGER) return;
      
      US_RESET();
      estadoUltrasonido = US_ESPERAR_ECO;
      break;
      
    case US_ESPERAR_ECO:
      if(US_LEER() == HIGH) {
        if(inicioEcoMicros == 0) {
          inicioEcoMicros = ahoraMicros;
        }
      } else {
        if(inicioEcoMicros == 0) return;
        
        unsigned long duracion = ahoraMicros - inicioEcoMicros;
        distanciaCm = (long)(duracion * 0.034f / 2.0f);
        inicioEcoMicros = 0;
        estadoUltrasonido = US_INACTIVO;
      }
      break;
  }
}

enum Comando {
  CMD_ADELANTE,
  CMD_ATRAS,
  CMD_IZQUIERDA,
  CMD_DERECHA,
  CMD_PARAR,
  CMD_DESCONOCIDO
};

Comando parsearComando(const String& mensaje) {
  if(mensaje.indexOf("forward") >= 0 || mensaje.indexOf("adelante") >= 0) return CMD_ADELANTE;
  if(mensaje.indexOf("back") >= 0 || mensaje.indexOf("atras") >= 0) return CMD_ATRAS;
  if(mensaje.indexOf("left") >= 0 || mensaje.indexOf("izquierda") >= 0) return CMD_IZQUIERDA;
  if(mensaje.indexOf("right") >= 0 || mensaje.indexOf("derecha") >= 0) return CMD_DERECHA;
  if(mensaje.indexOf("stop") >= 0 || mensaje.indexOf("parar") >= 0) return CMD_PARAR;
  return CMD_DESCONOCIDO;
}

void ejecutarComando(Comando cmd) {
  switch(cmd) {
    case CMD_ADELANTE:   moverAdelante();   break;
    case CMD_ATRAS:      moverAtras();      break;
    case CMD_IZQUIERDA:  girarIzquierda();  break;
    case CMD_DERECHA:    girarDerecha();    break;
    case CMD_PARAR:      detenerTodo();     break;
    case CMD_DESCONOCIDO: break;
  }
}

void callbackMqtt(char* topic, byte* payload, unsigned int longitud) {
  String mensaje;
  for(unsigned int i = 0; i < longitud; i++) {
    mensaje += (char)payload[i];
  }
  ejecutarComando(parsearComando(mensaje));
}

void reconectarMqtt() {
  while(!clienteMqtt.connected()) {
    if(!clienteMqtt.connect(MQTT_CLIENT_ID)) continue;
    clienteMqtt.subscribe(MQTT_TOPIC_COMANDO);
  }
}

void inicializarPWM() {
  ledcSetup(CANAL_PWM1, PWM_FREQ, PWM_RES);
  ledcSetup(CANAL_PWM2, PWM_FREQ, PWM_RES);
  ledcAttachPin(PIN_PWM1, CANAL_PWM1);
  ledcAttachPin(PIN_PWM2, CANAL_PWM2);
}

void inicializarWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED);
}

void inicializarMQTT() {
  clienteMqtt.setServer(MQTT_BROKER, MQTT_PORT);
  clienteMqtt.setCallback(callbackMqtt);
  reconectarMqtt();
}

void inicializarServidor() {
  servidor.on("/", []() {
    servidor.send(200, "text/html", generarPaginaHTML());
  });
  
  servidor.on("/adelante", []() {
    moverAdelante();
    servidor.send(200, "text/plain", "OK");
  });
  
  servidor.on("/atras", []() {
    moverAtras();
    servidor.send(200, "text/plain", "OK");
  });
  
  servidor.on("/izquierda", []() {
    girarIzquierda();
    servidor.send(200, "text/plain", "OK");
  });
  
  servidor.on("/derecha", []() {
    girarDerecha();
    servidor.send(200, "text/plain", "OK");
  });
  
  servidor.on("/parar", []() {
    detenerTodo();
    servidor.send(200, "text/plain", "OK");
  });
  
  servidor.begin();
}

void setup() {
  INICIALIZAR_PINES();
  inicializarPWM();
  detenerTodo();
  inicializarWiFi();
  inicializarMQTT();
  inicializarServidor();
  ultimoEnvioTelemetria = millis();
}

void actualizarConexiones() {
  if(!clienteMqtt.connected()) {
    reconectarMqtt();
  }
  clienteMqtt.loop();
  servidor.handleClient();
}

void actualizarTelemetria() {
  unsigned long ahora = millis();
  if(ahora - ultimoEnvioTelemetria < INTERVALO_TELEMETRIA) return;
  
  enviarTelemetria();
  ultimoEnvioTelemetria = ahora;
}

void loop() {
  actualizarConexiones();
  procesarUltrasonido();
  actualizarTelemetria();
}