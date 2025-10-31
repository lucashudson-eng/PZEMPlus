/**
 * @file PZEM003017.cpp
 * @brief Implementation of PZEM-003/017 energy monitoring module class
 * @author Lucas Hudson
 * @date 2025
 */

#include "PZEM003017.h"

#if defined(__AVR_ATmega328P__)
/**
 * @brief Constructor for AVR ATmega328P (Arduino Uno/Nano) with SoftwareSerial
 */
PZEM003017::PZEM003017(SoftwareSerial &serial, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr) {
}
#else
/**
 * @brief Constructor for ESP32/ESP8266 with HardwareSerial
 */
PZEM003017::PZEM003017(HardwareSerial &serial, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(-1), _txPin(-1) {
}

/**
 * @brief Constructor for ESP32/ESP8266 with HardwareSerial and custom pins
 */
PZEM003017::PZEM003017(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(rxPin), _txPin(txPin) {
}
#endif

/**
 * @brief Initialize serial communication
 */
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

/**
 * @brief Read voltage from device
 */
float PZEM003017::readVoltage() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 1, data)) {
        return data[0] * PZEM_VOLTAGE_RESOLUTION;
    }
    return NAN; // Error value
}

/**
 * @brief Read current from device
 */
float PZEM003017::readCurrent() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_REG, 1, data)) {
        return data[0] * PZEM_CURRENT_RESOLUTION;
    }
    return NAN; // Error value
}

/**
 * @brief Read power from device
 */
float PZEM003017::readPower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_LOW_REG, 2, data)) {
        uint32_t powerRaw = combineRegisters(data[0], data[1]);
        return powerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN; // Error value
}

/**
 * @brief Read energy from device
 */
float PZEM003017::readEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ENERGY_LOW_REG, 2, data)) {
        uint32_t energyRaw = combineRegisters(data[0], data[1]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN; // Error value
}

/**
 * @brief Read high voltage alarm status
 */
bool PZEM003017::readHighVoltageAlarm() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_HIGH_VOLTAGE_ALARM_REG, 1, data)) {
        return (data[0] == 0xFFFF); // 0xFFFF = active alarm
    }
    return false; // In case of error, assume no alarm
}

/**
 * @brief Read low voltage alarm status
 */
bool PZEM003017::readLowVoltageAlarm() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_LOW_VOLTAGE_ALARM_REG, 1, data)) {
        return (data[0] == 0xFFFF); // 0xFFFF = active alarm
    }
    return false; // In case of error, assume no alarm
}

/**
 * @brief Read all measurements at once
 */
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


/**
 * @brief Set high voltage alarm threshold
 */
bool PZEM003017::setHighVoltageAlarm(float threshold) {
    uint16_t thresholdRaw = (uint16_t)(threshold / PZEM_HIGH_VOLTAGE_ALARM_RESOLUTION); // Convert volts to raw value
    return writeSingleRegister(_slaveAddr, PZEM_HIGH_VOLTAGE_THRESHOLD_REG, thresholdRaw);
}

/**
 * @brief Set low voltage alarm threshold
 */
bool PZEM003017::setLowVoltageAlarm(float threshold) {
    uint16_t thresholdRaw = (uint16_t)(threshold / PZEM_LOW_VOLTAGE_ALARM_RESOLUTION); // Convert volts to raw value
    return writeSingleRegister(_slaveAddr, PZEM_LOW_VOLTAGE_THRESHOLD_REG, thresholdRaw);
}

/**
 * @brief Set device slave address
 */
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

/**
 * @brief Set current range (PZEM-017 only)
 */
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

/**
 * @brief Get high voltage alarm threshold
 */
float PZEM003017::getHighVoltageAlarm() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_HIGH_VOLTAGE_THRESHOLD_REG, 1, data)) {
        return data[0] * PZEM_HIGH_VOLTAGE_ALARM_RESOLUTION; // Convert raw value to volts
    }
    return NAN; // Error value
}

/**
 * @brief Get low voltage alarm threshold
 */
float PZEM003017::getLowVoltageAlarm() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_LOW_VOLTAGE_THRESHOLD_REG, 1, data)) {
        return data[0] * PZEM_LOW_VOLTAGE_ALARM_RESOLUTION; // Convert raw value to volts
    }
    return NAN; // Error value
}

/**
 * @brief Get device slave address
 */
uint8_t PZEM003017::getAddress() {
    uint16_t data[1];
    if (readHoldingRegisters(_slaveAddr, PZEM_ADDRESS_REG, 1, data)) {
        return data[0];
    }
    return _slaveAddr; // Return current address in case of error
}

/**
 * @brief Get current range (PZEM-017 only)
 */
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

/**
 * @brief Reset energy counter
 */
bool PZEM003017::resetEnergy() {
    return RS485::resetEnergy(_slaveAddr);
}
