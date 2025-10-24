#ifndef CONTROLMOTORES_H
#define CONTROLMOTORES_H

#include "Config.h"

class ControlMotores {
public:
  void configurarPines();
  void adelante();
  void atras();
  void izquierda();
  void derecha();
  void detener();
};

extern ControlMotores motores;

#endif
