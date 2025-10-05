#include "PZEM004T.h"

// Constructor
#if defined(__AVR_ATmega328P__)
PZEM004T::PZEM004T(SoftwareSerial &serial, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr), _sampleTimeMs(SAMPLE_TIME), _lastReadTime(0) {
}
#else
PZEM004T::PZEM004T(HardwareSerial &serial, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr), _sampleTimeMs(SAMPLE_TIME), _lastReadTime(0), _rxPin(-1), _txPin(-1) {
}
PZEM004T::PZEM004T(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr) 
    : RS485(&serial), _slaveAddr(slaveAddr), _sampleTimeMs(SAMPLE_TIME), _lastReadTime(0), _rxPin(rxPin), _txPin(txPin) {
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
}

// Read voltage
float PZEM004T::readVoltage() {
    if (_sampleTimeMs <= 0) {
        // Sample time disabled, read directly
        uint16_t data[1];
        if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 1, data)) {
            return data[0] * PZEM_VOLTAGE_RESOLUTION;
        }
        return -1.0f; // Error value
    } else {
        // Use sample time, call readAll to get cached data
        float voltage, current, power, energy, frequency, powerFactor;
        if (readAll(&voltage, &current, &power, &energy, &frequency, &powerFactor)) {
            return voltage;
        }
        return -1.0f; // Error value
    }
}

// Read current
float PZEM004T::readCurrent() {
    if (_sampleTimeMs <= 0) {
        // Sample time disabled, read directly
        uint16_t data[2];
        if (readInputRegisters(_slaveAddr, PZEM_CURRENT_LOW_REG, 2, data)) {
            uint32_t currentRaw = combineRegisters(data[0], data[1]);
            return currentRaw * PZEM_CURRENT_RESOLUTION;
        }
        return -1.0f; // Error value
    } else {
        // Use sample time, call readAll to get cached data
        float voltage, current, power, energy, frequency, powerFactor;
        if (readAll(&voltage, &current, &power, &energy, &frequency, &powerFactor)) {
            return current;
        }
        return -1.0f; // Error value
    }
}

// Read power
float PZEM004T::readPower() {
    if (_sampleTimeMs <= 0) {
        // Sample time disabled, read directly
        uint16_t data[2];
        if (readInputRegisters(_slaveAddr, PZEM_POWER_LOW_REG, 2, data)) {
            uint32_t powerRaw = combineRegisters(data[0], data[1]);
            return powerRaw * PZEM_POWER_RESOLUTION;
        }
        return -1.0f; // Error value
    } else {
        // Use sample time, call readAll to get cached data
        float voltage, current, power, energy, frequency, powerFactor;
        if (readAll(&voltage, &current, &power, &energy, &frequency, &powerFactor)) {
            return power;
        }
        return -1.0f; // Error value
    }
}

// Read energy
float PZEM004T::readEnergy() {
    if (_sampleTimeMs <= 0) {
        // Sample time disabled, read directly
        uint16_t data[2];
        if (readInputRegisters(_slaveAddr, PZEM_ENERGY_LOW_REG, 2, data)) {
            uint32_t energyRaw = combineRegisters(data[0], data[1]);
            return energyRaw * PZEM_ENERGY_RESOLUTION;
        }
        return -1.0f; // Error value
    } else {
        // Use sample time, call readAll to get cached data
        float voltage, current, power, energy, frequency, powerFactor;
        if (readAll(&voltage, &current, &power, &energy, &frequency, &powerFactor)) {
            return energy;
        }
        return -1.0f; // Error value
    }
}

// Read frequency
float PZEM004T::readFrequency() {
    if (_sampleTimeMs <= 0) {
        // Sample time disabled, read directly
        uint16_t data[1];
        if (readInputRegisters(_slaveAddr, PZEM_FREQUENCY_REG, 1, data)) {
            return data[0] * PZEM_FREQUENCY_RESOLUTION;
        }
        return -1.0f; // Error value
    } else {
        // Use sample time, call readAll to get cached data
        float voltage, current, power, energy, frequency, powerFactor;
        if (readAll(&voltage, &current, &power, &energy, &frequency, &powerFactor)) {
            return frequency;
        }
        return -1.0f; // Error value
    }
}

// Read power factor
float PZEM004T::readPowerFactor() {
    if (_sampleTimeMs <= 0) {
        // Sample time disabled, read directly
        uint16_t data[1];
        if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_REG, 1, data)) {
            return data[0] * PZEM_POWER_FACTOR_RESOLUTION;
        }
        return -1.0f; // Error value
    } else {
        // Use sample time, call readAll to get cached data
        float voltage, current, power, energy, frequency, powerFactor;
        if (readAll(&voltage, &current, &power, &energy, &frequency, &powerFactor)) {
            return powerFactor;
        }
        return -1.0f; // Error value
    }
}

// Read power alarm status
bool PZEM004T::readPowerAlarm() {
    if (_sampleTimeMs <= 0) {
        // Sample time disabled, read directly
        uint16_t data[1];
        if (readInputRegisters(_slaveAddr, PZEM_ALARM_STATUS_REG, 1, data)) {
            return (data[0] == 0xFFFF); // 0xFFFF = active alarm
        }
        return false; // In case of error, assume no alarm
    } else {
        // Use sample time, read alarm directly
        uint16_t data[1];
        if (readInputRegisters(_slaveAddr, PZEM_ALARM_STATUS_REG, 1, data)) {
            return (data[0] == 0xFFFF); // 0xFFFF = active alarm
        }
        return false; // In case of error, assume no alarm
    }
}

// Read all measurements at once
bool PZEM004T::readAll(float* voltage, float* current, float* power, 
                       float* energy, float* frequency, float* powerFactor) {
    unsigned long currentTime = millis();
    
    // Check if we need to read new data based on sample time
    if (_sampleTimeMs <= 0 || (currentTime - _lastReadTime) >= _sampleTimeMs) {
        uint16_t data[9];
        
        if (!readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 9, data)) {
            return false;
        }
        
        // Process data according to manual and cache it
        _cachedVoltage = data[0] * PZEM_VOLTAGE_RESOLUTION;
        
        uint32_t currentRaw = combineRegisters(data[1], data[2]);
        _cachedCurrent = currentRaw * PZEM_CURRENT_RESOLUTION;
        
        uint32_t powerRaw = combineRegisters(data[3], data[4]);
        _cachedPower = powerRaw * PZEM_POWER_RESOLUTION;
        
        uint32_t energyRaw = combineRegisters(data[5], data[6]);
        _cachedEnergy = energyRaw * PZEM_ENERGY_RESOLUTION;
        
        _cachedFrequency = data[7] * PZEM_FREQUENCY_RESOLUTION;
        _cachedPowerFactor = data[8] * PZEM_POWER_FACTOR_RESOLUTION;
        
        _lastReadTime = currentTime;
    }
    
    // Return cached or newly read data
    *voltage = _cachedVoltage;
    *current = _cachedCurrent;
    *power = _cachedPower;
    *energy = _cachedEnergy;
    *frequency = _cachedFrequency;
    *powerFactor = _cachedPowerFactor;
    
    return true;
}

// Set sample time
void PZEM004T::setSampleTime(unsigned long sampleTimeMs) {
    _sampleTimeMs = sampleTimeMs;
}

// Set alarm threshold
bool PZEM004T::setPowerAlarm(float threshold) {
    uint16_t thresholdRaw = (uint16_t)(threshold / PZEM_POWER_ALARM_RESOLUTION); // Convert watts to raw value
    return writeSingleRegister(_slaveAddr, PZEM_ALARM_THRESHOLD_REG, thresholdRaw);
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
    if (readHoldingRegisters(_slaveAddr, PZEM_ALARM_THRESHOLD_REG, 1, data)) {
        return data[0] * PZEM_POWER_ALARM_RESOLUTION; // Convert raw value to watts
    }
    return -1.0f; // Error value
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

