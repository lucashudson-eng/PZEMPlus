#ifndef PZEM004T_H
#define PZEM004T_H

#include "RS485.h"

class PZEM004T {
public:
    PZEM004T(Stream &serial);
private:
    Stream* _serial;
};

#endif // PZEM004T_H