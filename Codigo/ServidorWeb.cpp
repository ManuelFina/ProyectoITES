#include "ServidorWeb.h"
#include "CtrlRuedas.h"

void ConfigurarservidorWeb() {

  servidorHTTP.on("/adelante", [](){
    movimientoActual = ADELANTE;
    servidorHTTP.send(200, "text/plain", "OK");
  });

  servidorHTTP.on("/atras", [](){
    movimientoActual = ATRAS;
    servidorHTTP.send(200, "text/plain", "OK");
  });

  servidorHTTP.on("/izquierda", [](){
    movimientoActual = IZQ;
    servidorHTTP.send(200, "text/plain", "OK");
  });

  servidorHTTP.on("/derecha", [](){
    movimientoActual = DER;
    servidorHTTP.send(200, "text/plain", "OK");
  });

  servidorHTTP.on("/parar", [](){
    movimientoActual = PARADO;
    servidorHTTP.send(200, "text/plain", "OK");
  });

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
