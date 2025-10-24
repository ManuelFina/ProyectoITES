#pragma once
#include "Config.h"
#include "globals.h"

class ServidorWeb {
public:
  void configurar();
  void actualizar();

private:
  static void manejadorRaiz();
  static void manejadorAdelante();
  static void manejadorAtras();
  static void manejadorIzquierda();
  static void manejadorDerecha();
  static void manejadorParar();
  static String generarHTML();
};
