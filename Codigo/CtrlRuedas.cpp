#include "CtrlRuedas.h"
#include "Globals.h"   
#include "Config.h"    

void MovimientoTanque() {
  const long distanciaActual = distanciaCm;

  if (distanciaActual >= DISTANCIA_MINIMA) return;
  movimientoActual = PARADO;
  return;


  switch (movimientoActual) {
    case ADELANTE:    AVANZAR;         break;
    case ATRAS:       RETROCEDER;      break;
    case IZQ:         ROTAR_IZQUIERDA; break;
    case DER:         ROTAR_DERECHA;   break;
    case PARADO:
    default:          DETENER_MOTORES; break;
  }
}
