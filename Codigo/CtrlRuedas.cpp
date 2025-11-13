// CtrlRuedas.cpp
#include "CtrlRuedas.h"
#include "Globals.h"   // movimientoActual, distanciaCm
#include "Config.h"    // macros AVANZAR/RETROCEDER/..., DISTANCIA_MINIMA

void MovimientoTanque() {
  // Copia local (evita leer varias veces el volatile)
  const long distanciaActual = distanciaCm;

  // Freno de seguridad por obstáculo (solo si la lectura es válida)
  if (distanciaActual >= 0 && distanciaActual < DISTANCIA_MINIMA) {
    DETENER_MOTORES;
    movimientoActual = PARADO;
    return;
  }

  // FSM de movimiento
  switch (movimientoActual) {
    case ADELANTE:    AVANZAR;         break;
    case ATRAS:       RETROCEDER;      break;
    case IZQ:         ROTAR_IZQUIERDA; break;
    case DER:         ROTAR_DERECHA;   break;
    case PARADO:
    default:          DETENER_MOTORES; break;
  }
}
