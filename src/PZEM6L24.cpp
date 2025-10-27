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
        int32_t powerRaw = combineRegistersSigned(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readReactivePower(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_POWER_REG + (phase * 2), 2, data, false)) {
        int32_t powerRaw = combineRegistersSigned(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readApparentPower(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_POWER_REG + (phase * 2), 2, data, false)) {
        int32_t powerRaw = combineRegistersSigned(data[0], data[1]);
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
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readReactiveEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_REG + (phase * 2), 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readApparentEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_REG + (phase * 2), 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
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
        int32_t powerRaw = combineRegistersSigned(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readReactivePower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_POWER_COMBINED_REG, 2, data, false)) {
        int32_t powerRaw = combineRegistersSigned(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readApparentPower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_POWER_COMBINED_REG, 2, data, false)) {
        int32_t powerRaw = combineRegistersSigned(data[0], data[1]);
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
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readReactiveEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_COMBINED_REG, 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

float PZEM6L24::readApparentEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_COMBINED_REG, 2, data, false)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
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
        int32_t powerARaw = combineRegistersSigned(data[0], data[1]);
        int32_t powerBRaw = combineRegistersSigned(data[2], data[3]);
        int32_t powerCRaw = combineRegistersSigned(data[4], data[5]);
        
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
        int32_t powerARaw = combineRegistersSigned(data[0], data[1]);
        int32_t powerBRaw = combineRegistersSigned(data[2], data[3]);
        int32_t powerCRaw = combineRegistersSigned(data[4], data[5]);
        
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
        int32_t powerARaw = combineRegistersSigned(data[0], data[1]);
        int32_t powerBRaw = combineRegistersSigned(data[2], data[3]);
        int32_t powerCRaw = combineRegistersSigned(data[4], data[5]);
        
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
        uint32_t energyARaw = combineRegisters(data[0], data[1]);
        uint32_t energyBRaw = combineRegisters(data[2], data[3]);
        uint32_t energyCRaw = combineRegisters(data[4], data[5]);
        
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
        uint32_t energyARaw = combineRegisters(data[0], data[1]);
        uint32_t energyBRaw = combineRegisters(data[2], data[3]);
        uint32_t energyCRaw = combineRegisters(data[4], data[5]);
        
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
        uint32_t energyARaw = combineRegisters(data[0], data[1]);
        uint32_t energyBRaw = combineRegisters(data[2], data[3]);
        uint32_t energyCRaw = combineRegisters(data[4], data[5]);
        
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

// Set baudrate
bool PZEM6L24::setBaudrate(uint8_t baudrate) {
    // Read current connection type to preserve it
    uint16_t currentData[1];
    uint8_t currentConnectionType = 0;
    if (readHoldingRegisters(_slaveAddr, PZEM_BAUDRATE_TYPE_REG, 1, currentData, false)) {
        currentConnectionType = (currentData[0] >> 8) & 0xFF;
    }
    
    // Prepare the data to write - baudrate in low byte, preserve connection type in high byte
    uint16_t data[1];
    data[0] = (currentConnectionType << 8) | baudrate;
    
    // Use writeMultipleRegisters to write to register 0x0001
    return writeMultipleRegisters(_slaveAddr, PZEM_BAUDRATE_TYPE_REG, 1, data, false);
}


// Set baudrate and connection type together
bool PZEM6L24::setBaudrateAndConnectionType(uint8_t baudrate, uint8_t connectionType) {
    // Prepare the data to write - baudrate in low byte, connectionType in high byte
    uint16_t data[1];
    data[0] = (connectionType << 8) | baudrate;
    
    // Use writeMultipleRegisters to write to register 0x0001
    return writeMultipleRegisters(_slaveAddr, PZEM_BAUDRATE_TYPE_REG, 1, data, false);
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
uint8_t PZEM6L24::getBaudrate() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_BAUDRATE_TYPE_REG, 1, data, false)) {
        // Return the low byte of the register (baudrate)
        return data[0] & 0xFF;
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

// Control methods
bool PZEM6L24::resetEnergy(uint8_t phaseOption) {
    return RS485::resetEnergy(_slaveAddr, phaseOption);
}