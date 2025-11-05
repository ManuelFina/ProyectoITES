#include "ServidorWeb.h"
#include "ControlMotores.h"

//acoplar esto a la fsm

void ConfigurarservidorWeb() {
  servidorHTTP.on("/", manejadorRaiz);
  servidorHTTP.on("/adelante", manejadorAdelante);
  servidorHTTP.on("/atras", manejadorAtras);
  servidorHTTP.on("/izquierda", manejadorIzquierda);
  servidorHTTP.on("/derecha", manejadorDerecha);
  servidorHTTP.on("/parar", manejadorParar);
  servidorHTTP.begin();
}

void ActualizarservidorWeb() {
  servidorHTTP.handleClient();
}

void manejadorRaiz() {
  servidorHTTP.send(200, "text/html", generarHTML());
}

String generarHTML() {
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
