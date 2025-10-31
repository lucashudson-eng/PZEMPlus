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
 * @param phase Phase number (0=A, 1=B, 2=C)
 * @return Voltage in volts, or NAN on error
 */
float PZEM6L24::readVoltage(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG + phase, 1, data, false)) {
        return data[0] * PZEM_VOLTAGE_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readCurrent(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_REG + phase, 1, data, false)) {
        return data[0] * PZEM_CURRENT_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readFrequency(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_FREQUENCY_REG + phase, 1, data, false)) {
        return data[0] * PZEM_FREQUENCY_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readActivePower(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_POWER_REG + (phase * 2), 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readReactivePower(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_POWER_REG + (phase * 2), 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readApparentPower(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_POWER_REG + (phase * 2), 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

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

float PZEM6L24::readActiveEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_REG + (phase * 2), 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readReactiveEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_REG + (phase * 2), 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readApparentEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_REG + (phase * 2), 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

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

float PZEM6L24::readCurrentPhaseAngle(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_PHASE_REG + phase, 1, data, false)) {
        return data[0] * PZEM_PHASE_RESOLUTION;
    }
    return NAN;
}

// Combined measurements
float PZEM6L24::readActivePower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_POWER_COMBINED_REG, 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readReactivePower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_POWER_COMBINED_REG, 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readApparentPower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_POWER_COMBINED_REG, 2, data, false)) {
        int32_t powerRaw = combineRegisters(data[0], data[1], true);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readPowerFactor() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_C_COMBINED_REG, 1, data, false)) {
        // Low byte is combined phase
        uint8_t factorRaw = data[0] & 0xFF;
        return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readActiveEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_COMBINED_REG, 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readReactiveEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_COMBINED_REG, 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readApparentEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_COMBINED_REG, 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1], true);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

// Multi-phase reading methods
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


// Set slave address
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

// Set baudrate and connection type together
bool PZEM6L24::setBaudrateAndConnectionType(uint8_t baudrate, uint8_t connectionType, bool forceBaudrate) {
    // Prepare the data to write - baudrate in low byte, connectionType in high byte
    uint16_t data[1];
    data[0] = (connectionType << 8) | baudrate;
    
    // Use writeMultipleRegisters to write to register 0x0001
    bool success = writeMultipleRegisters(_slaveAddr, PZEM_BAUDRATE_TYPE_REG, 1, data, false);
    
    // If successful, change the serial baudrate
    if (success || forceBaudrate) {
        // Convert baudrate code to actual baudrate value
        uint32_t newBaudrate;
        switch (baudrate) {
            case PZEM_BAUDRATE_2400:   newBaudrate = 2400;   break;
            case PZEM_BAUDRATE_4800:   newBaudrate = 4800;   break;
            case PZEM_BAUDRATE_9600:   newBaudrate = 9600;   break;
            case PZEM_BAUDRATE_19200:  newBaudrate = 19200;  break;
            case PZEM_BAUDRATE_38400:  newBaudrate = 38400;  break;
            case PZEM_BAUDRATE_57600:  newBaudrate = 57600;  break;
            case PZEM_BAUDRATE_115200: newBaudrate = 115200; break;
            default: newBaudrate = 9600; break; // Default fallback
        }
        
        // Reinitialize serial with new baudrate
        #if defined(__AVR_ATmega328P__)
            ((SoftwareSerial*)getSerial())->begin(newBaudrate);
        #else
            if (_rxPin != -1 && _txPin != -1) {
                ((HardwareSerial*)getSerial())->begin(newBaudrate, SERIAL_8N1, _rxPin, _txPin);
            } else {
                ((HardwareSerial*)getSerial())->begin(newBaudrate);
            }
        #endif
        clearBuffer();
    }
    
    return success;
}

// Set frequency type
bool PZEM6L24::setFrequency(uint8_t frequency) {
    // Prepare the data to write - frequency goes in the low byte of register 0x0002
    uint16_t data[1];
    
    // Set low byte to frequency value
    data[0] = frequency;
    
    // Use writeMultipleRegisters to write to register 0x0002
    return writeMultipleRegisters(_slaveAddr, PZEM_FREQUENCY_SYSTEM_REG, 1, data, false);
}

// Get software hardware or software addressing settings
bool PZEM6L24::getSoftwareHardwareSettings() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_ADDRESS_REG, 1, data, false)) {
        // Return true if using software addressing (low byte = 1)
        return (data[0] & 0xFF) == 1;
    }
    return false;
}

// Get slave address
uint8_t PZEM6L24::getAddress() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_ADDRESS_REG, 1, data, false)) {
        // Return the high byte of the register (address)
        return (data[0] >> 8) & 0xFF;
    }
    return 0; // Return 0 if failed to read
}

// Get baudrate
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
    return 0; // Return 0 if failed to read
}

// Get connection type
uint8_t PZEM6L24::getConnectionType() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_BAUDRATE_TYPE_REG, 1, data, false)) {
        // Return the high byte of the register (connection type)
        return (data[0] >> 8) & 0xFF;
    }
    return 0; // Return 0 if failed to read
}

// Get frequency type
uint8_t PZEM6L24::getFrequency() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_FREQUENCY_SYSTEM_REG, 1, data, false)) {
        // Return the low byte of the register (frequency type)
        return data[0] & 0xFF;
    }
    return 0; // Return 0 if failed to read
}

/**
 * @brief Reset energy counter(s)
 */
bool PZEM6L24::resetEnergy(uint8_t phaseOption) {
    return RS485::resetEnergy(_slaveAddr, phaseOption);
}