/**
 * @file PZEM017.h
 * @brief PZEM-017 energy monitoring module class
 * @author Lucas Hudson
 * @date 2025
 */

#ifndef PZEM017_H
#define PZEM017_H

#include "PZEM003.h"

#define PZEM_CURRENT_RANGE_REG           0x0003  ///< Current range register address

/**
 * @defgroup PZEM017CurrentRanges PZEM-017 Current Range Options
 * @brief Current range values for PZEM-017
 * @{
 */
#define PZEM_CURRENT_RANGE_100A           0x0000  ///< 100A current range
#define PZEM_CURRENT_RANGE_50A            0x0001  ///< 50A current range
#define PZEM_CURRENT_RANGE_200A           0x0002  ///< 200A current range
#define PZEM_CURRENT_RANGE_300A           0x0003  ///< 300A current range
/** @} */

/**
 * @class PZEM017
 * @brief Class for interfacing with PZEM-017 single-phase energy monitoring modules
 * 
 * This class provides methods to read voltage, current, power, and energy from
 * PZEM-017 devices via Modbus-RTU protocol.
 * It extends the PZEM003 class to add support for current range.
 */
class PZEM017 : public PZEM003 {
public:
    /**
     * @brief Set current range (PZEM-017 only)
     * @param range Current range value (50, 100, 200, or 300 amperes)
     * @return true if successful, false otherwise
     */
    bool setCurrentRange(uint16_t range = 100);

    /**
     * @brief Get current range (PZEM-017 only)
     * @return Current range in amperes (50, 100, 200, or 300), or NAN on error
     */
    float getCurrentRange();
};

#endif // PZEM017_H
