#include "ServidorWeb.h"
#include "ControlMotores.h"

void ServidorWeb::configurar() {
  servidorHTTP.on("/", manejadorRaiz);
  servidorHTTP.on("/adelante", manejadorAdelante);
  servidorHTTP.on("/atras", manejadorAtras);
  servidorHTTP.on("/izquierda", manejadorIzquierda);
  servidorHTTP.on("/derecha", manejadorDerecha);
  servidorHTTP.on("/parar", manejadorParar);
  servidorHTTP.begin();
}

void ServidorWeb::actualizar() {
  servidorHTTP.handleClient();
}

void ServidorWeb::manejadorRaiz() {
  servidorHTTP.send(200, "text/html", generarHTML());
}

void ServidorWeb::manejadorAdelante() {
  controlMotores.adelante();
  servidorHTTP.send(200, "text/plain", "OK");
}

void ServidorWeb::manejadorAtras() {
  controlMotores.atras();
  servidorHTTP.send(200, "text/plain", "OK");
}

void ServidorWeb::manejadorIzquierda() {
  controlMotores.izquierda();
  servidorHTTP.send(200, "text/plain", "OK");
}

void ServidorWeb::manejadorDerecha() {
  controlMotores.derecha();
  servidorHTTP.send(200, "text/plain", "OK");
}

void ServidorWeb::manejadorParar() {
  controlMotores.detener();
  servidorHTTP.send(200, "text/plain", "OK");
}

String ServidorWeb::generarHTML() {
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
