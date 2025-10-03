#include "PZEM004T.h"

PZEM004T::PZEM004T(Stream &serial) {
    _serial = &serial;  // Guarda ponteiro
}