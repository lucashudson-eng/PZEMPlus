/**
 * @file PZEM6L24.cpp
 * @brief Implementation of PZEM-6L24 three-phase energy monitoring module class
 * @author Lucas Hudson
 * @date 2025
 */

#include "PZEM6L24.h"

#if defined(__AVR_ATmega328P__)
/**
 * @brief Constructor for AVR ATmega328P (Arduino Uno/Nano) with SoftwareSerial
 */
PZEM6L24::PZEM6L24(SoftwareSerial &serial, uint8_t slaveAddr)
    : RS485(&serial), _slaveAddr(slaveAddr) {
}
#else
/**
 * @brief Constructor for ESP32/ESP8266 with HardwareSerial
 */
PZEM6L24::PZEM6L24(HardwareSerial &serial, uint8_t slaveAddr)
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(-1), _txPin(-1) {
}

/**
 * @brief Constructor for ESP32/ESP8266 with HardwareSerial and custom pins
 */
PZEM6L24::PZEM6L24(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr)
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(rxPin), _txPin(txPin) {
}
#endif

/**
 * @brief Initialize serial communication
 */
void PZEM6L24::begin(uint32_t baudrate) {
    #if defined(__AVR_ATmega328P__)
        ((SoftwareSerial*)getSerial())->begin(baudrate);
    #else
        if (_rxPin != -1 && _txPin != -1) {
            ((HardwareSerial*)getSerial())->begin(baudrate, SERIAL_8N1, _rxPin, _txPin);
        } else {
            ((HardwareSerial*)getSerial())->begin(baudrate);
        }
    #endif
    clearBuffer();
}

/**
 * @brief Read voltage for a specific phase
 */
float PZEM6L24::readVoltage(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG + phase, 1, data, false)) {
        return data[0] * PZEM_VOLTAGE_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read current for a specific phase
 */
float PZEM6L24::readCurrent(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_REG + phase, 1, data, false)) {
        return data[0] * PZEM_CURRENT_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read frequency for a specific phase
 */
float PZEM6L24::readFrequency(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_FREQUENCY_REG + phase, 1, data, false)) {
        return data[0] * PZEM_FREQUENCY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read active power for a specific phase
 */
float PZEM6L24::readActivePower(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_POWER_REG + (phase * 2), 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read reactive power for a specific phase
 */
float PZEM6L24::readReactivePower(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_POWER_REG + (phase * 2), 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read apparent power for a specific phase
 */
float PZEM6L24::readApparentPower(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_POWER_REG + (phase * 2), 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read power factor for a specific phase
 */
float PZEM6L24::readPowerFactor(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[1];
    if (phase == 0 || phase == 1) {
        // A and B phases are in the same register
        if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_A_B_REG, 1, data, false)) {
            if (phase == 0) {
                // High byte is A phase
                uint8_t factorRaw = (data[0] >> 8) & 0xFF;
                return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
            } else {
                // Low byte is B phase
                uint8_t factorRaw = data[0] & 0xFF;
                return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
            }
        }
    } else if (phase == 2) {
        // C phase is in the second register
        if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_C_COMBINED_REG, 1, data, false)) {
            // High byte is C phase
            uint8_t factorRaw = (data[0] >> 8) & 0xFF;
            return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
        }
    }
    return NAN;
}

/**
 * @brief Read active energy for a specific phase
 */
float PZEM6L24::readActiveEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_REG + (phase * 2), 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read reactive energy for a specific phase
 */
float PZEM6L24::readReactiveEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_REG + (phase * 2), 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read apparent energy for a specific phase
 */
float PZEM6L24::readApparentEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_REG + (phase * 2), 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read voltage phase angle for a specific phase
 */
float PZEM6L24::readVoltagePhaseAngle(uint8_t phase) {
    if (phase > 2) return NAN;
    
    if (phase == 0) {
        // Phase A is the reference, so angle is 0°
        return 0.0f;
    }
    
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_PHASE_REG + (phase - 1), 1, data, false)) {
        return data[0] * PZEM_PHASE_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read current phase angle for a specific phase
 */
float PZEM6L24::readCurrentPhaseAngle(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_PHASE_REG + phase, 1, data, false)) {
        return data[0] * PZEM_PHASE_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read combined active power (all three phases)
 */
float PZEM6L24::readActivePower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_POWER_COMBINED_REG, 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read combined reactive power (all three phases)
 */
float PZEM6L24::readReactivePower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_POWER_COMBINED_REG, 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read combined apparent power (all three phases)
 */
float PZEM6L24::readApparentPower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_POWER_COMBINED_REG, 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read combined power factor (all three phases)
 */
float PZEM6L24::readPowerFactor() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_C_COMBINED_REG, 1, data, false)) {
        // Low byte is combined phase
        uint8_t factorRaw = data[0] & 0xFF;
        return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read combined active energy (all three phases)
 */
float PZEM6L24::readActiveEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_COMBINED_REG, 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read combined reactive energy (all three phases)
 */
float PZEM6L24::readReactiveEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_COMBINED_REG, 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read combined apparent energy (all three phases)
 */
float PZEM6L24::readApparentEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_COMBINED_REG, 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read voltage for all three phases simultaneously
 */
void PZEM6L24::readVoltage(float& voltageA, float& voltageB, float& voltageC) {
    uint16_t data[3];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 3, data, false)) {
        voltageA = data[0] * PZEM_VOLTAGE_RESOLUTION;
        voltageB = data[1] * PZEM_VOLTAGE_RESOLUTION;
        voltageC = data[2] * PZEM_VOLTAGE_RESOLUTION;
    } else {
        voltageA = voltageB = voltageC = NAN;
    }
}

/**
 * @brief Read current for all three phases simultaneously
 */
void PZEM6L24::readCurrent(float& currentA, float& currentB, float& currentC) {
    uint16_t data[3];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_REG, 3, data, false)) {
        currentA = data[0] * PZEM_CURRENT_RESOLUTION;
        currentB = data[1] * PZEM_CURRENT_RESOLUTION;
        currentC = data[2] * PZEM_CURRENT_RESOLUTION;
    } else {
        currentA = currentB = currentC = NAN;
    }
}

/**
 * @brief Read frequency for all three phases simultaneously
 */
void PZEM6L24::readFrequency(float& frequencyA, float& frequencyB, float& frequencyC) {
    uint16_t data[3];
    if (readInputRegisters(_slaveAddr, PZEM_FREQUENCY_REG, 3, data, false)) {
        frequencyA = data[0] * PZEM_FREQUENCY_RESOLUTION;
        frequencyB = data[1] * PZEM_FREQUENCY_RESOLUTION;
        frequencyC = data[2] * PZEM_FREQUENCY_RESOLUTION;
    } else {
        frequencyA = frequencyB = frequencyC = NAN;
    }
}

/**
 * @brief Read voltage and current for all three phases simultaneously
 */
void PZEM6L24::readVoltageCurrent(float& voltageA, float& voltageB, float& voltageC, float& currentA, float& currentB, float& currentC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 6, data, false)) {
        voltageA = data[0] * PZEM_VOLTAGE_RESOLUTION;
        voltageB = data[1] * PZEM_VOLTAGE_RESOLUTION;
        voltageC = data[2] * PZEM_VOLTAGE_RESOLUTION;
        currentA = data[3] * PZEM_CURRENT_RESOLUTION;
        currentB = data[4] * PZEM_CURRENT_RESOLUTION;
        currentC = data[5] * PZEM_CURRENT_RESOLUTION;
    } else {
        voltageA = voltageB = voltageC = NAN;
        currentA = currentB = currentC = NAN;
    }
}

/**
 * @brief Read active power for all three phases simultaneously
 */
void PZEM6L24::readActivePower(float& powerA, float& powerB, float& powerC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_POWER_REG, 6, data, false)) {
        int32_t powerARaw = combineRegisters(data[0], data[1], true);
        int32_t powerBRaw = combineRegisters(data[2], data[3], true);
        int32_t powerCRaw = combineRegisters(data[4], data[5], true);
        
        powerA = powerARaw * PZEM_POWER_RESOLUTION;
        powerB = powerBRaw * PZEM_POWER_RESOLUTION;
        powerC = powerCRaw * PZEM_POWER_RESOLUTION;
    } else {
        powerA = powerB = powerC = NAN;
    }
}

/**
 * @brief Read reactive power for all three phases simultaneously
 */
void PZEM6L24::readReactivePower(float& powerA, float& powerB, float& powerC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_POWER_REG, 6, data, false)) {
        int32_t powerARaw = combineRegisters(data[0], data[1], true);
        int32_t powerBRaw = combineRegisters(data[2], data[3], true);
        int32_t powerCRaw = combineRegisters(data[4], data[5], true);
        
        powerA = powerARaw * PZEM_POWER_RESOLUTION;
        powerB = powerBRaw * PZEM_POWER_RESOLUTION;
        powerC = powerCRaw * PZEM_POWER_RESOLUTION;
    } else {
        powerA = powerB = powerC = NAN;
    }
}

/**
 * @brief Read apparent power for all three phases simultaneously
 */
void PZEM6L24::readApparentPower(float& powerA, float& powerB, float& powerC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_POWER_REG, 6, data, false)) {
        int32_t powerARaw = combineRegisters(data[0], data[1], true);
        int32_t powerBRaw = combineRegisters(data[2], data[3], true);
        int32_t powerCRaw = combineRegisters(data[4], data[5], true);
        
        powerA = powerARaw * PZEM_POWER_RESOLUTION;
        powerB = powerBRaw * PZEM_POWER_RESOLUTION;
        powerC = powerCRaw * PZEM_POWER_RESOLUTION;
    } else {
        powerA = powerB = powerC = NAN;
    }
}

/**
 * @brief Read power factor for all three phases simultaneously
 */
void PZEM6L24::readPowerFactor(float& factorA, float& factorB, float& factorC) {
    // Read both power factor registers in one call for efficiency
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_A_B_REG, 2, data, false)) {
        // Register 0x0026: High byte = A phase, Low byte = B phase
        uint8_t factorARaw = (data[0] >> 8) & 0xFF;
        uint8_t factorBRaw = data[0] & 0xFF;
        
        // Register 0x0027: High byte = C phase
        uint8_t factorCRaw = (data[1] >> 8) & 0xFF;
        
        factorA = factorARaw * PZEM_POWER_FACTOR_RESOLUTION;
        factorB = factorBRaw * PZEM_POWER_FACTOR_RESOLUTION;
        factorC = factorCRaw * PZEM_POWER_FACTOR_RESOLUTION;
    } else {
        // Error case - set all to -1
        factorA = NAN;
        factorB = NAN;
        factorC = NAN;
    }
}

/**
 * @brief Read active energy for all three phases simultaneously
 */
void PZEM6L24::readActiveEnergy(float& energyA, float& energyB, float& energyC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_REG, 6, data, false)) {
        uint32_t energyARaw = combineRegisters(data[0], data[1], true);
        uint32_t energyBRaw = combineRegisters(data[2], data[3], true);
        uint32_t energyCRaw = combineRegisters(data[4], data[5], true);
        
        energyA = energyARaw * PZEM_ENERGY_RESOLUTION;
        energyB = energyBRaw * PZEM_ENERGY_RESOLUTION;
        energyC = energyCRaw * PZEM_ENERGY_RESOLUTION;
    } else {
        energyA = energyB = energyC = NAN;
    }
}

/**
 * @brief Read reactive energy for all three phases simultaneously
 */
void PZEM6L24::readReactiveEnergy(float& energyA, float& energyB, float& energyC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_REG, 6, data, false)) {
        uint32_t energyARaw = combineRegisters(data[0], data[1], true);
        uint32_t energyBRaw = combineRegisters(data[2], data[3], true);
        uint32_t energyCRaw = combineRegisters(data[4], data[5], true);
        
        energyA = energyARaw * PZEM_ENERGY_RESOLUTION;
        energyB = energyBRaw * PZEM_ENERGY_RESOLUTION;
        energyC = energyCRaw * PZEM_ENERGY_RESOLUTION;
    } else {
        energyA = energyB = energyC = NAN;
    }
}

/**
 * @brief Read apparent energy for all three phases simultaneously
 */
void PZEM6L24::readApparentEnergy(float& energyA, float& energyB, float& energyC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_REG, 6, data, false)) {
        uint32_t energyARaw = combineRegisters(data[0], data[1], true);
        uint32_t energyBRaw = combineRegisters(data[2], data[3], true);
        uint32_t energyCRaw = combineRegisters(data[4], data[5], true);
        
        energyA = energyARaw * PZEM_ENERGY_RESOLUTION;
        energyB = energyBRaw * PZEM_ENERGY_RESOLUTION;
        energyC = energyCRaw * PZEM_ENERGY_RESOLUTION;
    } else {
        energyA = energyB = energyC = NAN;
    }
}

/**
 * @brief Read voltage phase angle for all three phases simultaneously
 */
void PZEM6L24::readVoltagePhaseAngle(float& angleA, float& angleB, float& angleC) {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_PHASE_REG, 2, data, false)) {
        angleA = 0.0f; // Phase A is reference
        angleB = data[0] * PZEM_PHASE_RESOLUTION;
        angleC = data[1] * PZEM_PHASE_RESOLUTION;
    } else {
        angleA = angleB = angleC = NAN;
    }
}

/**
 * @brief Read current phase angle for all three phases simultaneously
 */
void PZEM6L24::readCurrentPhaseAngle(float& angleA, float& angleB, float& angleC) {
    uint16_t data[3];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_PHASE_REG, 3, data, false)) {
        angleA = data[0] * PZEM_PHASE_RESOLUTION;
        angleB = data[1] * PZEM_PHASE_RESOLUTION;
        angleC = data[2] * PZEM_PHASE_RESOLUTION;
    } else {
        angleA = angleB = angleC = NAN;
    }
}

/**
 * @brief Set device slave address
 */
bool PZEM6L24::setAddress(uint8_t address) {
    // Validate address range
    if (address > 0xF7) {
        return false; // Invalid address
    }
    
    // Prepare the data to write
    uint16_t data[1];
    
    if (address == 0) {
        // Hardware addressing: low byte = 0, high byte = 1
        data[0] = 0x0100;
    } else {
        // Software addressing: low byte = 1, high byte = address
        data[0] = (address << 8) | 0x01;
    }
    
    // Use writeMultipleRegisters to write to register 0x0000
    return writeMultipleRegisters(_slaveAddr, PZEM_ADDRESS_REG, 1, data, false);
}

/**
 * @brief Set baudrate and connection type simultaneously
 */
bool PZEM6L24::setBaudrateAndConnectionType(uint32_t baudrate, uint8_t connectionType, bool forceBaudrate) {
    // Convert baudrate value to baudrate code
    uint8_t baudrateCode;
    switch (baudrate) {
        case 2400:   baudrateCode = PZEM_BAUDRATE_2400;   break;
        case 4800:   baudrateCode = PZEM_BAUDRATE_4800;   break;
        case 9600:   baudrateCode = PZEM_BAUDRATE_9600;   break;
        case 19200:  baudrateCode = PZEM_BAUDRATE_19200;  break;
        case 38400:  baudrateCode = PZEM_BAUDRATE_38400;  break;
        case 57600:  baudrateCode = PZEM_BAUDRATE_57600;  break;
        case 115200: baudrateCode = PZEM_BAUDRATE_115200; break;
        default: return false; // Invalid baudrate value
    }
    
    // Prepare the data to write - baudrate in low byte, connectionType in high byte
    uint16_t data[1];
    data[0] = (connectionType << 8) | baudrateCode;
    
    // Use writeMultipleRegisters to write to register 0x0001
    bool success = writeMultipleRegisters(_slaveAddr, PZEM_BAUDRATE_TYPE_REG, 1, data, false);
    
    // If successful, change the serial baudrate
    if (success || forceBaudrate) {
        // Reinitialize serial with new baudrate
        #if defined(__AVR_ATmega328P__)
            ((SoftwareSerial*)getSerial())->begin(baudrate);
        #else
            if (_rxPin != -1 && _txPin != -1) {
                ((HardwareSerial*)getSerial())->begin(baudrate, SERIAL_8N1, _rxPin, _txPin);
            } else {
                ((HardwareSerial*)getSerial())->begin(baudrate);
            }
        #endif
        clearBuffer();
    }
    
    return success;
}

/**
 * @brief Set AC frequency system (50Hz or 60Hz)
 */
bool PZEM6L24::setFrequency(uint8_t frequency) {
    // Convert frequency value to frequency code
    uint8_t frequencyCode;
    switch (frequency) {
        case 50: frequencyCode = PZEM_FREQUENCY_50HZ; break;
        case 60: frequencyCode = PZEM_FREQUENCY_60HZ; break;
        default: return false; // Invalid frequency value
    }
    
    // Prepare the data to write - frequency goes in the low byte of register 0x0002
    uint16_t data[1];
    data[0] = frequencyCode;
    
    // Use writeMultipleRegisters to write to register 0x0002
    return writeMultipleRegisters(_slaveAddr, PZEM_FREQUENCY_SYSTEM_REG, 1, data, false);
}

/**
 * @brief Get addressing mode (software or hardware)
 */
bool PZEM6L24::getSoftwareHardwareSettings() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_ADDRESS_REG, 1, data, false)) {
        // Return true if using software addressing (low byte = 1)
        return (data[0] & 0xFF) == 1;
    }
    return false;
}

/**
 * @brief Get device slave address
 */
uint8_t PZEM6L24::getAddress() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_ADDRESS_REG, 1, data, false)) {
        // Return the high byte of the register (address)
        return (uint8_t)((data[0] >> 8) & 0xFF);
    }
    return 0xFF; // Error value
}

/**
 * @brief Get current baudrate setting
 */
uint32_t PZEM6L24::getBaudrate() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_BAUDRATE_TYPE_REG, 1, data, false)) {
        // Get the baudrate code from low byte
        uint8_t baudrateCode = data[0] & 0xFF;
        
        // Convert baudrate code to actual baudrate value
        switch (baudrateCode) {
            case PZEM_BAUDRATE_2400:   return 2400;
            case PZEM_BAUDRATE_4800:   return 4800;
            case PZEM_BAUDRATE_9600:   return 9600;
            case PZEM_BAUDRATE_19200:  return 19200;
            case PZEM_BAUDRATE_38400:  return 38400;
            case PZEM_BAUDRATE_57600:  return 57600;
            case PZEM_BAUDRATE_115200: return 115200;
            default: return 0; // Unknown baudrate code
        }
    }
    return 0; // Error value
}

/**
 * @brief Get connection type setting
 */
uint8_t PZEM6L24::getConnectionType() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_BAUDRATE_TYPE_REG, 1, data, false)) {
        // Return the high byte of the register (connection type)
        return (uint8_t)((data[0] >> 8) & 0xFF);
    }
    return 0xFF; // Error value
}

/**
 * @brief Get AC frequency system setting
 */
uint8_t PZEM6L24::getFrequency() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_FREQUENCY_SYSTEM_REG, 1, data, false)) {
        // Get the frequency code from low byte
        uint8_t frequencyCode = data[0] & 0xFF;
        
        // Convert frequency code to actual frequency value
        switch (frequencyCode) {
            case PZEM_FREQUENCY_50HZ: return 50;
            case PZEM_FREQUENCY_60HZ: return 60;
            default: return 0; // Unknown frequency code
        }
    }
    return 0; // Error value
}

/**
 * @brief Reset energy counter(s)
 */
bool PZEM6L24::resetEnergy(uint8_t phaseOption) {
    return RS485::resetEnergy(_slaveAddr, phaseOption);
}