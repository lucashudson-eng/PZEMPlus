#include "PZEM003017.h"

// Constructors
#if defined(__AVR_ATmega328P__)
PZEM003017::PZEM003017(SoftwareSerial &serial, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr) {
}
#else
PZEM003017::PZEM003017(HardwareSerial &serial, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(-1), _txPin(-1) {
}
PZEM003017::PZEM003017(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(rxPin), _txPin(txPin) {
}
#endif

void PZEM003017::begin(uint32_t baudrate){
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
float PZEM003017::readVoltage() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 1, data)) {
        return data[0] * PZEM_VOLTAGE_RESOLUTION;
    }
    return NAN; // Error value
}

// Read current
float PZEM003017::readCurrent() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_REG, 1, data)) {
        return data[0] * PZEM_CURRENT_RESOLUTION;
    }
    return NAN; // Error value
}

// Read power
float PZEM003017::readPower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_LOW_REG, 2, data)) {
        uint32_t powerRaw = combineRegisters(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN; // Error value
}

// Read energy
float PZEM003017::readEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ENERGY_LOW_REG, 2, data)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN; // Error value
}

// Read high voltage alarm status
bool PZEM003017::readHighVoltageAlarm() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_HIGH_VOLTAGE_ALARM_REG, 1, data)) {
        return (data[0] == 0xFFFF); // 0xFFFF = active alarm
    }
    return false; // In case of error, assume no alarm
}

// Read low voltage alarm status
bool PZEM003017::readLowVoltageAlarm() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_LOW_VOLTAGE_ALARM_REG, 1, data)) {
        return (data[0] == 0xFFFF); // 0xFFFF = active alarm
    }
    return false; // In case of error, assume no alarm
}

// Read all measurements at once
bool PZEM003017::readAll(float* voltage, float* current, float* power, float* energy) {
    uint16_t data[6];
    
    if (!readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 6, data)) {
        return false;
    }
    
    // Process data according to manual
    *voltage = data[0] * PZEM_VOLTAGE_RESOLUTION;
    *current = data[1] * PZEM_CURRENT_RESOLUTION;
    
    uint32_t powerRaw = combineRegisters(data[2], data[3]);
    *power = powerRaw * PZEM_POWER_RESOLUTION;
    
    uint32_t energyRaw = combineRegisters(data[4], data[5]);
    *energy = energyRaw * PZEM_ENERGY_RESOLUTION;
    
    return true;
}


// Set high voltage alarm
bool PZEM003017::setHighVoltageAlarm(float threshold) {
    uint16_t thresholdRaw = (uint16_t)(threshold / PZEM_HIGH_VOLTAGE_ALARM_RESOLUTION); // Convert volts to raw value
    return writeSingleRegister(_slaveAddr, PZEM_HIGH_VOLTAGE_THRESHOLD_REG, thresholdRaw);
}

// Set low voltage alarm
bool PZEM003017::setLowVoltageAlarm(float threshold) {
    uint16_t thresholdRaw = (uint16_t)(threshold / PZEM_LOW_VOLTAGE_ALARM_RESOLUTION); // Convert volts to raw value
    return writeSingleRegister(_slaveAddr, PZEM_LOW_VOLTAGE_THRESHOLD_REG, thresholdRaw);
}

// Set slave address
bool PZEM003017::setAddress(uint8_t newAddress) {
    if (newAddress < 0x01 || newAddress > 0xF7) {
        return false; // Invalid address
    }
    
    bool success = writeSingleRegister(_slaveAddr, PZEM_ADDRESS_REG, newAddress);
    if (success) {
        _slaveAddr = newAddress; // Update local address
    }
    return success;
}

// Set current range (PZEM-017 only)
bool PZEM003017::setCurrentRange(uint16_t range) {
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

// Get high voltage alarm
float PZEM003017::getHighVoltageAlarm() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_HIGH_VOLTAGE_THRESHOLD_REG, 1, data)) {
        return data[0] * PZEM_HIGH_VOLTAGE_ALARM_RESOLUTION; // Convert raw value to volts
    }
    return NAN; // Error value
}

// Get low voltage alarm
float PZEM003017::getLowVoltageAlarm() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_LOW_VOLTAGE_THRESHOLD_REG, 1, data)) {
        return data[0] * PZEM_LOW_VOLTAGE_ALARM_RESOLUTION; // Convert raw value to volts
    }
    return NAN; // Error value
}

// Get slave address
uint8_t PZEM003017::getAddress() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_ADDRESS_REG, 1, data)) {
        return data[0];
    }
    return _slaveAddr; // Return current address in case of error
}

// Get current range (PZEM-017 only)
uint16_t PZEM003017::getCurrentRange() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_CURRENT_RANGE_REG, 1, data)) {
        // Convert internal constant to range value
        switch (data[0]) {
            case PZEM_CURRENT_RANGE_100A:
                return 100;
            case PZEM_CURRENT_RANGE_50A:
                return 50;
            case PZEM_CURRENT_RANGE_200A:
                return 200;
            case PZEM_CURRENT_RANGE_300A:
                return 300;
            default:
                return 0; // Unknown range value
        }
    }
    return 0; // Error value
}

// Reset energy
bool PZEM003017::resetEnergy() {
    return RS485::resetEnergy(_slaveAddr);
}
