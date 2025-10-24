#pragma once
#include "Config.h"
#include "globals.h"

class ClienteMQTT {
public:
  void configurar();
  void actualizar();
  void reconectar();
  void enviarMedicion();
};
