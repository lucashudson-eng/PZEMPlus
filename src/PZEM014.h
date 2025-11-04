/**
 * @file PZEM014.h
 * @brief PZEM-014 energy monitoring module class
 * @author Lucas Hudson
 * @date 2025
 */

#ifndef PZEM014_H
#define PZEM014_H

#include "PZEM004T.h"

/**
 * @class PZEM014
 * @brief Class for interfacing with PZEM-014 single-phase energy monitoring modules
 * 
 * This class provides methods to read voltage, current, power, and energy from
 * PZEM-014 devices via Modbus-RTU protocol.
 * It extends the PZEM004T class and provides the same functionality.
 */
class PZEM014 : public PZEM004T {};

#endif // PZEM014_H
