#include "PZEM004T.h"

// Constructor
PZEM004T::PZEM004T(Stream &serial, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr) {
}

// Read voltage
float PZEM004T::readVoltage() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 1, data)) {
        return data[0] * PZEM_VOLTAGE_RESOLUTION;
    }
    return -1.0f; // Error value
}

// Read current
float PZEM004T::readCurrent() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_LOW_REG, 2, data)) {
        uint32_t currentRaw = combineRegisters(data[0], data[1]);
        return currentRaw * PZEM_CURRENT_RESOLUTION;
    }
    return -1.0f; // Error value
}

// Read power
float PZEM004T::readPower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_LOW_REG, 2, data)) {
        uint32_t powerRaw = combineRegisters(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return -1.0f; // Error value
}

// Read energy
float PZEM004T::readEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ENERGY_LOW_REG, 2, data)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return -1.0f; // Error value
}

// Read frequency
float PZEM004T::readFrequency() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_FREQUENCY_REG, 1, data)) {
        return data[0] * PZEM_FREQUENCY_RESOLUTION;
    }
    return -1.0f; // Error value
}

// Read power factor
float PZEM004T::readPowerFactor() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_REG, 1, data)) {
        return data[0] * PZEM_POWER_FACTOR_RESOLUTION;
    }
    return -1.0f; // Error value
}

// Read alarm status
bool PZEM004T::readAlarmStatus() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_ALARM_STATUS_REG, 1, data)) {
        return (data[0] == 0xFFFF); // 0xFFFF = active alarm
    }
    return false; // In case of error, assume no alarm
}

// Read all measurements at once
bool PZEM004T::readAll(float* voltage, float* current, float* power, 
                       float* energy, float* frequency, float* powerFactor, bool* alarm) {
    uint16_t data[10];
    
    if (!readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 10, data)) {
        return false;
    }
    
    // Process data according to manual
    *voltage = data[0] * PZEM_VOLTAGE_RESOLUTION;
    
    uint32_t currentRaw = combineRegisters(data[1], data[2]);
    *current = currentRaw * PZEM_CURRENT_RESOLUTION;
    
    uint32_t powerRaw = combineRegisters(data[3], data[4]);
    *power = powerRaw * PZEM_POWER_RESOLUTION;
    
    uint32_t energyRaw = combineRegisters(data[5], data[6]);
    *energy = energyRaw * PZEM_ENERGY_RESOLUTION;
    
    *frequency = data[7] * PZEM_FREQUENCY_RESOLUTION;
    *powerFactor = data[8] * PZEM_POWER_FACTOR_RESOLUTION;
    *alarm = (data[9] == 0xFFFF);
    
    return true;
}

// Set alarm threshold
bool PZEM004T::setAlarmThreshold(uint16_t threshold) {
    return writeSingleRegister(_slaveAddr, PZEM_ALARM_THRESHOLD_REG, threshold);
}

// Set slave address
bool PZEM004T::setAddress(uint8_t newAddress) {
    if (newAddress < 0x01 || newAddress > 0xF7) {
        return false; // Invalid address
    }
    
    bool success = writeSingleRegister(_slaveAddr, PZEM_ADDRESS_REG, newAddress);
    if (success) {
        _slaveAddr = newAddress; // Update local address
    }
    return success;
}

// Get alarm threshold
uint16_t PZEM004T::getAlarmThreshold() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_ALARM_THRESHOLD_REG, 1, data)) {
        return data[0];
    }
    return 0; // Error value
}

// Get slave address
uint8_t PZEM004T::getAddress() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_ADDRESS_REG, 1, data)) {
        return data[0];
    }
    return _slaveAddr; // Return current address in case of error
}

// Reset energy
bool PZEM004T::resetEnergy() {
    return RS485::resetEnergy(_slaveAddr);
}

// Internal method to combine 16-bit registers into 32 bits
uint32_t PZEM004T::combineRegisters(uint16_t low, uint16_t high) {
    return ((uint32_t)high << 16) | low;
}
