#pragma once
#include <Arduino.h>   // asegura definición de uint8_t, String, etc.

void ConfigurarClienteMQTT();
void ActualizarClienteMQTT();
void EnviarMedicionMQTT();

// Firma correcta para PubSubClient en ESP32 core 3.x:
void CallbackMQTT(char* topic, uint8_t* payload, unsigned int length);
