#include <WiFi.h>
#include <PubSubClient.h>

#define WIFI_SSID        "Alumnos"
#define WIFI_PASSWORD    ""
#define MQTT_SERVER      "broker.hivemq.com"
#define MQTT_PORT        1883

#define NOMBRE           "Leonardo"
#define APELLIDO         "Renzi"

#define LED_PIN          2
#define CONFIG_LED       pinMode(LED_PIN, OUTPUT)
#define ENCENDER_LED     digitalWrite(LED_PIN, HIGH)
#define APAGAR_LED       digitalWrite(LED_PIN, LOW)

WiFiClient ESP_CLIENT;
PubSubClient CLIENT(ESP_CLIENT);

unsigned long LAST_BLINK = 0;
bool LED_STATE = false;

// ---------- CALLBACK MQTT ----------
void MQTT_CALLBACK(char* topic, byte* payload, unsigned int length) {
    Serial.print("[MQTT] Mensaje recibido en ");
    Serial.print(topic);
    Serial.print(": ");

    String mensaje;
    for (unsigned int i = 0; i < length; i++) {
        mensaje += (char)payload[i];
    }
    Serial.println(mensaje);
}

// ---------- RECONEXIÓN MQTT ----------
void RECONNECT_MQTT() {
    if (!CLIENT.connected()) {
        Serial.print("[MQTT] Intentando conexión...");
        if (CLIENT.connect("ESP32Client")) {
            Serial.println(" conectado!");

            String topicSub = String(APELLIDO) + "ites/msg";
            CLIENT.subscribe(topicSub.c_str());
            Serial.print("[MQTT] Suscrito a: ");
            Serial.println(topicSub);

            String topicPub = String(APELLIDO) + "/ini";
            String mac = WiFi.macAddress();
            String mensaje = String(NOMBRE) + " " + String(APELLIDO) + " - MAC: " + mac;
            CLIENT.publish(topicPub.c_str(), mensaje.c_str());
            Serial.print("[MQTT] Publicado en ");
            Serial.print(topicPub);
            Serial.print(": ");
            Serial.println(mensaje);
        } else {
            Serial.print(" fallo rc=");
            Serial.print(CLIENT.state());
            Serial.println(" - Reintentando...");
        }
    }
}

// ---------- CONEXIÓN WIFI ----------
void SETUP_WIFI() {
    Serial.print("[WiFi] Conectando a ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

// ---------- SETUP ----------
void setup() {
    Serial.begin(115200);
    CONFIG_LED;

    SETUP_WIFI();
    // Conexión no bloqueante: revisamos en loop si se conecta
    while (WiFi.status() != WL_CONNECTED) {
        // sin delay, dejamos que el loop se encargue
        Serial.print(".");
    }
    Serial.println("\n[WiFi] Conectado a la red!");

    CLIENT.setServer(MQTT_SERVER, MQTT_PORT);
    CLIENT.setCallback(MQTT_CALLBACK);

    // Opcional: aumentar tamaño de mensajes MQTT
    // CLIENT.setBufferSize(1024);
}

// ---------- LOOP PRINCIPAL ----------
void loop() {
    unsigned long now = millis();

    // Blink no bloqueante: 100ms ON cada 2s
    if (LED_STATE && now - LAST_BLINK >= 100) {
        APAGAR_LED;
        LED_STATE = false;
        LAST_BLINK = now;
    } 
    else if (!LED_STATE && now - LAST_BLINK >= 2000) {
        ENCENDER_LED;
        LED_STATE = true;
        LAST_BLINK = now;
    }

    // Conexión WiFi no bloqueante
    if (WiFi.status() != WL_CONNECTED) {
        SETUP_WIFI();
        return; // Salimos hasta que se conecte
    }

    // Reintento MQTT
    RECONNECT_MQTT();
    CLIENT.loop();
}
