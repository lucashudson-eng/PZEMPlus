#include "PZEM004T.h"

// Constructors
#if defined(__AVR_ATmega328P__)
PZEM004T::PZEM004T(SoftwareSerial &serial, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr) {
}
#else
PZEM004T::PZEM004T(HardwareSerial &serial, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(-1), _txPin(-1) {
}
PZEM004T::PZEM004T(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(rxPin), _txPin(txPin) {
}
#endif

void PZEM004T::begin(uint32_t baudrate){
    #if defined(__AVR_ATmega328P__)
        ((SoftwareSerial*)getSerial())->begin(baudrate);
    #else
        if (_rxPin != -1 && _txPin != -1){
            ((HardwareSerial*)getSerial())->begin(baudrate, SERIAL_8N1, _rxPin, _txPin);
        } else {
            ((HardwareSerial*)getSerial())->begin(baudrate);
        }
    #endif
    clearBuffer();
}

// Read voltage
float PZEM004T::readVoltage() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 1, data)) {
        return data[0] * PZEM_VOLTAGE_RESOLUTION;
    }
    return NAN; // Error value
}

// Read current
float PZEM004T::readCurrent() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_LOW_REG, 2, data)) {
        uint32_t currentRaw = combineRegisters(data[0], data[1]);
        return currentRaw * PZEM_CURRENT_RESOLUTION;
    }
    return NAN; // Error value
}

// Read power
float PZEM004T::readPower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_LOW_REG, 2, data)) {
        uint32_t powerRaw = combineRegisters(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN; // Error value
}

// Read energy
float PZEM004T::readEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ENERGY_LOW_REG, 2, data)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN; // Error value
}

// Read frequency
float PZEM004T::readFrequency() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_FREQUENCY_REG, 1, data)) {
        return data[0] * PZEM_FREQUENCY_RESOLUTION;
    }
    return NAN; // Error value
}

// Read power factor
float PZEM004T::readPowerFactor() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_REG, 1, data)) {
        return data[0] * PZEM_POWER_FACTOR_RESOLUTION;
    }
    return NAN; // Error value
}

// Read power alarm status
bool PZEM004T::readPowerAlarm() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_ALARM_REG, 1, data)) {
        return (data[0] == 0xFFFF); // 0xFFFF = active alarm
    }
    return false; // In case of error, assume no alarm
}

// Read all measurements at once
bool PZEM004T::readAll(float* voltage, float* current, float* power, 
                       float* energy, float* frequency, float* powerFactor) {
    uint16_t data[9];
    
    if (!readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 9, data)) {
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
    
    return true;
}


// Set alarm threshold
bool PZEM004T::setPowerAlarm(float threshold) {
    uint16_t thresholdRaw = (uint16_t)(threshold / PZEM_POWER_ALARM_RESOLUTION); // Convert watts to raw value
    return writeSingleRegister(_slaveAddr, PZEM_POWER_THRESHOLD_REG, thresholdRaw);
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

// Get power alarm threshold
float PZEM004T::getPowerAlarm() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_POWER_THRESHOLD_REG, 1, data)) {
        return data[0] * PZEM_POWER_ALARM_RESOLUTION; // Convert raw value to watts
    }
    return NAN; // Error value
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

