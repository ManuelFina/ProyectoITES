#include <WiFi.h>
#include <PubSubClient.h>

#define WIFI_SSID       "TU_SSID"
#define WIFI_PASS       "TU_PASS"
#define MQTT_SERVER     "192.168.1.100"
#define MQTT_PORT       1883
#define CLIENT_ID       "esp32_tank_01"

#define PIN_IN1         16
#define PIN_IN2         17
#define PIN_IN3         18
#define PIN_IN4         19
#define PIN_ENA         25
#define PIN_ENB         26

#define PIN_TRIG        12
#define PIN_ECHO        13
#define PIN_VBAT        35   

#define PWM_FREQ       2000
#define PWM_RES        8
#define CH_A           0
#define CH_B           1

#define MOTOR_A_FWD()   digitalWrite(PIN_IN1, HIGH); digitalWrite(PIN_IN2, LOW)
#define MOTOR_A_BACK()  digitalWrite(PIN_IN1, LOW);  digitalWrite(PIN_IN2, HIGH)
#define MOTOR_A_STOP()  digitalWrite(PIN_IN1, LOW);  digitalWrite(PIN_IN2, LOW)

#define MOTOR_B_FWD()   digitalWrite(PIN_IN3, HIGH); digitalWrite(PIN_IN4, LOW)
#define MOTOR_B_BACK()  digitalWrite(PIN_IN3, LOW);  digitalWrite(PIN_IN4, HIGH)
#define MOTOR_B_STOP()  digitalWrite(PIN_IN3, LOW);  digitalWrite(PIN_IN4, LOW)

#define MOTOR_A_PWM(val) ledcWrite(CH_A, val)
#define MOTOR_B_PWM(val) ledcWrite(CH_B, val)

#define ULTRASONIC_TRIGGER() digitalWrite(PIN_TRIG, HIGH)
#define ULTRASONIC_RESET()   digitalWrite(PIN_TRIG, LOW)
#define ULTRASONIC_READ()    digitalRead(PIN_ECHO)

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastTelemetry = 0;

enum USState {US_IDLE, US_TRIGGER, US_WAIT_ECHO};
USState usState = US_IDLE;
unsigned long usStart = 0;
unsigned long echoStart = 0;
long lastDistance = -1;


void setupPins() {

  //abstraer hardware a macros
  pinMode(PIN_IN1, OUTPUT); pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT); pinMode(PIN_IN4, OUTPUT);
  ledcSetup(CH_A, PWM_FREQ, PWM_RES);
  ledcSetup(CH_B, PWM_FREQ, PWM_RES);
  ledcAttachPin(PIN_ENA, CH_A);
  ledcAttachPin(PIN_ENB, CH_B);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  ULTRASONIC_RESET();
}

void setup() {
  Serial.begin(115200);
  setupPins();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200); //refactorizar con millis
    Serial.print(".");
  }
  Serial.println(" OK");

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqttCallback);
  reconnectMQTT();
}


void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando MQTT...");
    if (client.connect(CLIENT_ID)) {
      Serial.println(" OK");
      client.subscribe("tank/command");
    } else {
      Serial.print(" fallo, rc=");
      Serial.println(client.state());
      delay(1000); //refactorizar con millis
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.print("CMD: "); Serial.println(msg);
  
  //refactorizar puede ser un swicht case
  if (msg.indexOf("forward") >= 0) driveForward(200);
  else if (msg.indexOf("back") >= 0) driveBack(200); 
  else if (msg.indexOf("left") >= 0) turnLeft(200);
  else if (msg.indexOf("right") >= 0) turnRight(200);
  else if (msg.indexOf("stop") >= 0) stopMotors();
}


void driveForward(int pwm) {
  MOTOR_A_FWD(); MOTOR_B_FWD();
  MOTOR_A_PWM(pwm); MOTOR_B_PWM(pwm);
}
void driveBack(int pwm) {
  MOTOR_A_BACK(); MOTOR_B_BACK();
  MOTOR_A_PWM(pwm); MOTOR_B_PWM(pwm);
}
void turnLeft(int pwm) {
  MOTOR_A_BACK(); MOTOR_B_FWD();
  MOTOR_A_PWM(pwm); MOTOR_B_PWM(pwm);
}
void turnRight(int pwm) {
  MOTOR_A_FWD(); MOTOR_B_BACK();
  MOTOR_A_PWM(pwm); MOTOR_B_PWM(pwm);
}
void stopMotors() {
  MOTOR_A_STOP(); MOTOR_B_STOP();
  MOTOR_A_PWM(0); MOTOR_B_PWM(0);
}

//refactorizar 
void handleUltrasonic() {
  unsigned long now = micros();

  switch (usState) {
    case US_IDLE:
      if (millis() - lastTelemetry > 500) { // cada 500ms
        ULTRASONIC_TRIGGER();
        usStart = now;
        usState = US_TRIGGER;
      }
      break;

    case US_TRIGGER:
      if (now - usStart >= 10) { 
        ULTRASONIC_RESET();
        usStart = now;
        usState = US_WAIT_ECHO;
      }
      break;

    case US_WAIT_ECHO:
      if (ULTRASONIC_READ() == HIGH) {
        echoStart = now;
      } else if (echoStart > 0) {
        long duration = now - echoStart;
        lastDistance = duration * 0.034 / 2; // cm
        echoStart = 0;
        usState = US_IDLE;
      }
      break;
  }
}


void sendTelemetry() {
  int vRaw = analogRead(PIN_VBAT);
  float voltage = (vRaw / 4095.0) * 3.3 * 2.0; 

  String payload = "{";
  payload += "\"distance\":" + String(lastDistance) + ",";
  payload += "\"voltage\":" + String(voltage, 2);
  payload += "}";
  client.publish("tank/telemetry/state", payload.c_str());
  Serial.println("Telemetry -> " + payload);
}

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  handleUltrasonic();

  unsigned long now = millis();
  if (now - lastTelemetry >= 2000) {
    sendTelemetry();
    lastTelemetry = now;
  }
}
