#include "CtrlRuedas.h"
#include "globals.h"

void MovimientoTanque(){
 switch (movimientoActual) {
    case ADELANTE:    AVANZAR;         break;
    case ATRAS:       RETROCEDER;      break;
    case IZQ:         ROTAR_IZQUIERDA; break;
    case DER:         ROTAR_DERECHA;   break;
    case PARADO: 
    default :         DETENER_MOTORES; break;
}

