#include "PZEM6L24.h"

// Constructors
#if defined(__AVR_ATmega328P__)
PZEM6L24::PZEM6L24(SoftwareSerial &serial, uint8_t slaveAddr)
    : RS485(&serial), _slaveAddr(slaveAddr) {
}
#else
PZEM6L24::PZEM6L24(HardwareSerial &serial, uint8_t slaveAddr)
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(-1), _txPin(-1) {
}

PZEM6L24::PZEM6L24(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr)
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(rxPin), _txPin(txPin) {
}
#endif

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

// Basic measurements by phase (0=A, 1=B, 2=C)
float PZEM6L24::readVoltage(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_VOLTAGE_REG + phase, 1, data)) {
        return data[0] * PZEM_VOLTAGE_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readCurrent(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_CURRENT_REG + phase, 1, data)) {
        return data[0] * PZEM_CURRENT_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readFrequency(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_FREQUENCY_REG + phase, 1, data)) {
        return data[0] * PZEM_FREQUENCY_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readActivePower(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_ACTIVE_POWER_REG + (phase * 2), 2, data)) {
        uint32_t powerRaw = combineRegisters(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readReactivePower(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_REACTIVE_POWER_REG + (phase * 2), 2, data)) {
        uint32_t powerRaw = combineRegisters(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readApparentPower(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_APPARENT_POWER_REG + (phase * 2), 2, data)) {
        uint32_t powerRaw = combineRegisters(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readPowerFactor(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[1];
    if (phase == 0 || phase == 1) {
        // A and B phases are in the same register
        if (readHoldingRegisters(_slaveAddr, PZEM_POWER_FACTOR_A_B_REG, 1, data)) {
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
        if (readHoldingRegisters(_slaveAddr, PZEM_POWER_FACTOR_C_COMBINED_REG, 1, data)) {
            // High byte is C phase
            uint8_t factorRaw = (data[0] >> 8) & 0xFF;
            return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
        }
    }
    return -1.0f;
}

float PZEM6L24::readActiveEnergy(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_REG + (phase * 2), 2, data)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readReactiveEnergy(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_REG + (phase * 2), 2, data)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readApparentEnergy(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_REG + (phase * 2), 2, data)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readVoltagePhaseAngle(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    if (phase == 0) {
        // Phase A is the reference, so angle is 0°
        return 0.0f;
    }
    
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_VOLTAGE_PHASE_REG + (phase - 1), 1, data)) {
        return data[0] * PZEM_PHASE_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readCurrentPhaseAngle(uint8_t phase) {
    if (phase > 2) return -1.0f;
    
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_CURRENT_PHASE_REG + phase, 1, data)) {
        return data[0] * PZEM_PHASE_RESOLUTION;
    }
    return -1.0f;
}

// Combined measurements
float PZEM6L24::readActivePower() {
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_ACTIVE_POWER_COMBINED_REG, 2, data)) {
        uint32_t powerRaw = combineRegisters(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readReactivePower() {
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_REACTIVE_POWER_COMBINED_REG, 2, data)) {
        uint32_t powerRaw = combineRegisters(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readApparentPower() {
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_APPARENT_POWER_COMBINED_REG, 2, data)) {
        uint32_t powerRaw = combineRegisters(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readPowerFactor() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_POWER_FACTOR_C_COMBINED_REG, 1, data)) {
        // Low byte is combined phase
        uint8_t factorRaw = data[0] & 0xFF;
        return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readActiveEnergy() {
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_COMBINED_REG, 2, data)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readReactiveEnergy() {
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_COMBINED_REG, 2, data)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return -1.0f;
}

float PZEM6L24::readApparentEnergy() {
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_COMBINED_REG, 2, data)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return -1.0f;
}

// Multi-phase reading methods
void PZEM6L24::readVoltage(float& voltageA, float& voltageB, float& voltageC) {
    uint16_t data[3];
    if (readHoldingRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 3, data)) {
        voltageA = data[0] * PZEM_VOLTAGE_RESOLUTION;
        voltageB = data[1] * PZEM_VOLTAGE_RESOLUTION;
        voltageC = data[2] * PZEM_VOLTAGE_RESOLUTION;
    } else {
        voltageA = voltageB = voltageC = -1.0f;
    }
}

void PZEM6L24::readCurrent(float& currentA, float& currentB, float& currentC) {
    uint16_t data[3];
    if (readHoldingRegisters(_slaveAddr, PZEM_CURRENT_REG, 3, data)) {
        currentA = data[0] * PZEM_CURRENT_RESOLUTION;
        currentB = data[1] * PZEM_CURRENT_RESOLUTION;
        currentC = data[2] * PZEM_CURRENT_RESOLUTION;
    } else {
        currentA = currentB = currentC = -1.0f;
    }
}

void PZEM6L24::readFrequency(float& frequencyA, float& frequencyB, float& frequencyC) {
    uint16_t data[3];
    if (readHoldingRegisters(_slaveAddr, PZEM_FREQUENCY_REG, 3, data)) {
        frequencyA = data[0] * PZEM_FREQUENCY_RESOLUTION;
        frequencyB = data[1] * PZEM_FREQUENCY_RESOLUTION;
        frequencyC = data[2] * PZEM_FREQUENCY_RESOLUTION;
    } else {
        frequencyA = frequencyB = frequencyC = -1.0f;
    }
}

void PZEM6L24::readActivePower(float& powerA, float& powerB, float& powerC) {
    uint16_t data[6];
    if (readHoldingRegisters(_slaveAddr, PZEM_ACTIVE_POWER_REG, 6, data)) {
        uint32_t powerARaw = combineRegisters(data[0], data[1]);
        uint32_t powerBRaw = combineRegisters(data[2], data[3]);
        uint32_t powerCRaw = combineRegisters(data[4], data[5]);
        
        powerA = powerARaw * PZEM_POWER_RESOLUTION;
        powerB = powerBRaw * PZEM_POWER_RESOLUTION;
        powerC = powerCRaw * PZEM_POWER_RESOLUTION;
    } else {
        powerA = powerB = powerC = -1.0f;
    }
}

void PZEM6L24::readReactivePower(float& powerA, float& powerB, float& powerC) {
    uint16_t data[6];
    if (readHoldingRegisters(_slaveAddr, PZEM_REACTIVE_POWER_REG, 6, data)) {
        uint32_t powerARaw = combineRegisters(data[0], data[1]);
        uint32_t powerBRaw = combineRegisters(data[2], data[3]);
        uint32_t powerCRaw = combineRegisters(data[4], data[5]);
        
        powerA = powerARaw * PZEM_POWER_RESOLUTION;
        powerB = powerBRaw * PZEM_POWER_RESOLUTION;
        powerC = powerCRaw * PZEM_POWER_RESOLUTION;
    } else {
        powerA = powerB = powerC = -1.0f;
    }
}

void PZEM6L24::readApparentPower(float& powerA, float& powerB, float& powerC) {
    uint16_t data[6];
    if (readHoldingRegisters(_slaveAddr, PZEM_APPARENT_POWER_REG, 6, data)) {
        uint32_t powerARaw = combineRegisters(data[0], data[1]);
        uint32_t powerBRaw = combineRegisters(data[2], data[3]);
        uint32_t powerCRaw = combineRegisters(data[4], data[5]);
        
        powerA = powerARaw * PZEM_POWER_RESOLUTION;
        powerB = powerBRaw * PZEM_POWER_RESOLUTION;
        powerC = powerCRaw * PZEM_POWER_RESOLUTION;
    } else {
        powerA = powerB = powerC = -1.0f;
    }
}

void PZEM6L24::readPowerFactor(float& factorA, float& factorB, float& factorC) {
    // Read both power factor registers in one call for efficiency
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_POWER_FACTOR_A_B_REG, 2, data)) {
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
        factorA = -1.0f;
        factorB = -1.0f;
        factorC = -1.0f;
    }
}

void PZEM6L24::readActiveEnergy(float& energyA, float& energyB, float& energyC) {
    uint16_t data[6];
    if (readHoldingRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_REG, 6, data)) {
        uint32_t energyARaw = combineRegisters(data[0], data[1]);
        uint32_t energyBRaw = combineRegisters(data[2], data[3]);
        uint32_t energyCRaw = combineRegisters(data[4], data[5]);
        
        energyA = energyARaw * PZEM_ENERGY_RESOLUTION;
        energyB = energyBRaw * PZEM_ENERGY_RESOLUTION;
        energyC = energyCRaw * PZEM_ENERGY_RESOLUTION;
    } else {
        energyA = energyB = energyC = -1.0f;
    }
}

void PZEM6L24::readReactiveEnergy(float& energyA, float& energyB, float& energyC) {
    uint16_t data[6];
    if (readHoldingRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_REG, 6, data)) {
        uint32_t energyARaw = combineRegisters(data[0], data[1]);
        uint32_t energyBRaw = combineRegisters(data[2], data[3]);
        uint32_t energyCRaw = combineRegisters(data[4], data[5]);
        
        energyA = energyARaw * PZEM_ENERGY_RESOLUTION;
        energyB = energyBRaw * PZEM_ENERGY_RESOLUTION;
        energyC = energyCRaw * PZEM_ENERGY_RESOLUTION;
    } else {
        energyA = energyB = energyC = -1.0f;
    }
}

void PZEM6L24::readApparentEnergy(float& energyA, float& energyB, float& energyC) {
    uint16_t data[6];
    if (readHoldingRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_REG, 6, data)) {
        uint32_t energyARaw = combineRegisters(data[0], data[1]);
        uint32_t energyBRaw = combineRegisters(data[2], data[3]);
        uint32_t energyCRaw = combineRegisters(data[4], data[5]);
        
        energyA = energyARaw * PZEM_ENERGY_RESOLUTION;
        energyB = energyBRaw * PZEM_ENERGY_RESOLUTION;
        energyC = energyCRaw * PZEM_ENERGY_RESOLUTION;
    } else {
        energyA = energyB = energyC = -1.0f;
    }
}

void PZEM6L24::readVoltagePhaseAngle(float& angleA, float& angleB, float& angleC) {
    uint16_t data[2];
    if (readHoldingRegisters(_slaveAddr, PZEM_VOLTAGE_PHASE_REG, 2, data)) {
        angleA = 0.0f; // Phase A is reference
        angleB = data[0] * PZEM_PHASE_RESOLUTION;
        angleC = data[1] * PZEM_PHASE_RESOLUTION;
    } else {
        angleA = angleB = angleC = -1.0f;
    }
}

void PZEM6L24::readCurrentPhaseAngle(float& angleA, float& angleB, float& angleC) {
    uint16_t data[3];
    if (readHoldingRegisters(_slaveAddr, PZEM_CURRENT_PHASE_REG, 3, data)) {
        angleA = data[0] * PZEM_PHASE_RESOLUTION;
        angleB = data[1] * PZEM_PHASE_RESOLUTION;
        angleC = data[2] * PZEM_PHASE_RESOLUTION;
    } else {
        angleA = angleB = angleC = -1.0f;
    }
}

// Reset energy method
bool PZEM6L24::resetEnergy(uint8_t phaseOption) {
    return RS485::resetEnergy(_slaveAddr, phaseOption);
}