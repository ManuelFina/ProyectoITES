#pragma once
#include <Arduino.h>  

void ConfigurarClienteMQTT();
void ActualizarClienteMQTT();
void EnviarMedicionMQTT();

void CallbackMQTT(char* topic, uint8_t* payload, unsigned int length);
