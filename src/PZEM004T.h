#ifndef PZEM004T_H
#define PZEM004T_H

#include "RS485.h"

// PZEM-004T register addresses
#define PZEM_VOLTAGE_REG         0x0000
#define PZEM_CURRENT_LOW_REG      0x0001
#define PZEM_POWER_LOW_REG        0x0003
#define PZEM_ENERGY_LOW_REG       0x0005
#define PZEM_FREQUENCY_REG        0x0007
#define PZEM_POWER_FACTOR_REG     0x0008
#define PZEM_POWER_ALARM_REG     0x0009

// Parameter registers
#define PZEM_POWER_THRESHOLD_REG  0x0001
#define PZEM_ADDRESS_REG          0x0002

// Resolutions according to manual
#define PZEM_VOLTAGE_RESOLUTION    0.1f
#define PZEM_CURRENT_RESOLUTION    0.001f
#define PZEM_POWER_RESOLUTION      0.1f
#define PZEM_POWER_ALARM_RESOLUTION 1.0f
#define PZEM_ENERGY_RESOLUTION     1.0f
#define PZEM_FREQUENCY_RESOLUTION  0.1f
#define PZEM_POWER_FACTOR_RESOLUTION 0.01f

class PZEM004T : public RS485 {
public:
    // Constructors
#if defined(__AVR_ATmega328P__) 
    PZEM004T(SoftwareSerial &serial, uint8_t slaveAddr = 0xF8);
#else
    PZEM004T(HardwareSerial &serial, uint8_t slaveAddr = 0xF8);
    PZEM004T(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr = 0xF8);
    #endif

    void begin(uint32_t baudrate = 9600);
    
    // Measurement methods
    float readVoltage();
    float readCurrent();
    float readPower();
    float readEnergy();
    float readFrequency();
    float readPowerFactor();
    bool readPowerAlarm();
    
    // Method to read all measurements at once
    bool readAll(float* voltage, float* current, float* power, float* energy, float* frequency, float* powerFactor);
    
    // Sample time control
    void setSampleTime(unsigned long sampleTimeMs);
    
    // Parameter methods
    bool setPowerAlarm(float threshold);
    bool setAddress(uint8_t newAddress);
    float getPowerAlarm();
    uint8_t getAddress();
    
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
    float _cachedFrequency;
    float _cachedPowerFactor;
};

#endif // PZEM004T_H