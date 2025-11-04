/**
 * @file PZEM017.cpp
 * @brief Implementation of PZEM-017 energy monitoring module class
 * @author Lucas Hudson
 * @date 2025
 */

#include "PZEM017.h"

/**
 * @brief Set current range (PZEM-017 only)
 */
bool PZEM017::setCurrentRange(uint16_t range) {
    uint16_t rangeValue;
    
    // Convert range value to internal constant
    switch (range) {
        case 100:
            rangeValue = PZEM_CURRENT_RANGE_100A;
            break;
        case 50:
            rangeValue = PZEM_CURRENT_RANGE_50A;
            break;
        case 200:
            rangeValue = PZEM_CURRENT_RANGE_200A;
            break;
        case 300:
            rangeValue = PZEM_CURRENT_RANGE_300A;
            break;
        default:
            return false; // Invalid range value
    }
    
    return writeSingleRegister(_slaveAddr, PZEM_CURRENT_RANGE_REG, rangeValue);
}

/**
 * @brief Get current range
 */
float PZEM017::getCurrentRange() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_CURRENT_RANGE_REG, 1, data)) {
        // Convert internal constant to range value
        switch (data[0]) {
            case PZEM_CURRENT_RANGE_100A:
                return 100.0f;
            case PZEM_CURRENT_RANGE_50A:
                return 50.0f;
            case PZEM_CURRENT_RANGE_200A:
                return 200.0f;
            case PZEM_CURRENT_RANGE_300A:
                return 300.0f;
            default:
                return NAN; // Unknown range value
        }
    }
    return NAN; // Error value
}