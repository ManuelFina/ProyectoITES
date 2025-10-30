#include "CtrlTanque.h"

void ComandoAdelante() {
  AVANZAR;
  servidorHTTP.send(200, "text/plain", "OK");
}

void ComandoAtras() {
  RETROCEDER;
  servidorHTTP.send(200, "text/plain", "OK");
}

void manejadorIzquierda() {
  controlMotores.izquierda();
  servidorHTTP.send(200, "text/plain", "OK");
}

void manejadorDerecha() {
  controlMotores.derecha();
  servidorHTTP.send(200, "text/plain", "OK");
}

void manejadorParar() {
  controlMotores.detener();
  servidorHTTP.send(200, "text/plain", "OK");
}
