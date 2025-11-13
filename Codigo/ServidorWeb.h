#ifndef SERVIDORWEB_H
#define SERVIDORWEB_H

#include <Arduino.h>
#include <WebServer.h>

void ConfigurarServidorWeb();
void ActualizarServidorWeb();
void manejadorRaiz();
String generarHTML();


#endif
