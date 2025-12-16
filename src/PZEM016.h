/**
 * @file PZEM016.h
 * @brief PZEM-016 energy monitoring module class
 * @author Lucas Hudson
 * @date 2025
 */

#ifndef PZEM016_H
#define PZEM016_H

#include "PZEM004T.h"

/**
 * @class PZEM016
 * @brief Class for interfacing with PZEM-016 single-phase energy monitoring modules
 * 
 * This class provides methods to read voltage, current, power, and energy from
 * PZEM-016 devices via Modbus-RTU protocol.
 * It extends the PZEM004T class and provides the same functionality.
 */
class PZEM016 : public PZEM004T {
public:
    using PZEM004T::PZEM004T;
};

#endif // PZEM016_H
