#ifndef PZEM003017_H
#define PZEM003017_H

#include "RS485.h"

// PZEM-003/017 register addresses for measurement results
#define PZEM_VOLTAGE_REG             0x0000
#define PZEM_CURRENT_REG             0x0001
#define PZEM_POWER_LOW_REG           0x0002
#define PZEM_POWER_HIGH_REG          0x0003
#define PZEM_ENERGY_LOW_REG           0x0004
#define PZEM_ENERGY_HIGH_REG          0x0005
#define PZEM_HIGH_VOLTAGE_ALARM_REG   0x0006
#define PZEM_LOW_VOLTAGE_ALARM_REG    0x0007

// PZEM-003/017 parameter registers
#define PZEM_HIGH_VOLTAGE_THRESHOLD_REG  0x0000
#define PZEM_LOW_VOLTAGE_THRESHOLD_REG   0x0001
#define PZEM_ADDRESS_REG                 0x0002
#define PZEM_CURRENT_RANGE_REG           0x0003

// Resolutions according to manual
#define PZEM_VOLTAGE_RESOLUTION           0.01f
#define PZEM_HIGH_VOLTAGE_ALARM_RESOLUTION 0.01f
#define PZEM_LOW_VOLTAGE_ALARM_RESOLUTION  0.01f
#define PZEM_CURRENT_RESOLUTION           0.01f
#define PZEM_POWER_RESOLUTION             0.1f
#define PZEM_ENERGY_RESOLUTION            1.0f

// Current range values for PZEM-017
#define PZEM_CURRENT_RANGE_100A           0x0000
#define PZEM_CURRENT_RANGE_50A            0x0001
#define PZEM_CURRENT_RANGE_200A           0x0002
#define PZEM_CURRENT_RANGE_300A           0x0003

// Define for default configurations
#define SAMPLE_TIME 0

class PZEM003017 : public RS485 {
public:
    // Constructor
    #if defined(__AVR_ATmega328P__) 
    PZEM003017(SoftwareSerial &serial, uint8_t slaveAddr = 0xF8);
    #else
    PZEM003017(HardwareSerial &serial, uint8_t slaveAddr = 0xF8);
    PZEM003017(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr = 0xF8);
    #endif

    void begin(uint32_t baudrate = 9600);
    
    // Measurement methods
    float readVoltage();
    float readCurrent();
    float readPower();
    float readEnergy();
    bool readHighVoltageAlarm();
    bool readLowVoltageAlarm();
    
    // Method to read all measurements at once
    bool readAll(float* voltage, float* current, float* power, float* energy);
    
    // Sample time control
    void setSampleTime(unsigned long sampleTimeMs);
    
    // Parameter methods
    bool setHighVoltageAlarm(float threshold);
    bool setLowVoltageAlarm(float threshold);
    bool setAddress(uint8_t newAddress);
    bool setCurrentRange(uint16_t range);
    float getHighVoltageAlarm();
    float getLowVoltageAlarm();
    uint8_t getAddress();
    uint16_t getCurrentRange();
    
    // Control methods
    bool resetEnergy();

private:
    uint8_t _slaveAddr;
#if !defined(__AVR_ATmega328P__)
    uint8_t _rxPin;
    uint8_t _txPin;
#endif

    // Sample time control
    unsigned long _sampleTimeMs;
    unsigned long _lastReadTime;
    
    // Cached data
    float _cachedVoltage;
    float _cachedCurrent;
    float _cachedPower;
    float _cachedEnergy;
};

#endif // PZEM003017_H
