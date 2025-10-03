#ifndef PZEM004T_H
#define PZEM004T_H

#include "RS485.h"

// PZEM-004T register addresses
#define PZEM_VOLTAGE_REG         0x0000
#define PZEM_CURRENT_LOW_REG      0x0001
#define PZEM_CURRENT_HIGH_REG     0x0002
#define PZEM_POWER_LOW_REG        0x0003
#define PZEM_POWER_HIGH_REG       0x0004
#define PZEM_ENERGY_LOW_REG       0x0005
#define PZEM_ENERGY_HIGH_REG      0x0006
#define PZEM_FREQUENCY_REG        0x0007
#define PZEM_POWER_FACTOR_REG     0x0008
#define PZEM_ALARM_STATUS_REG     0x0009

// Parameter registers
#define PZEM_ALARM_THRESHOLD_REG  0x0001
#define PZEM_ADDRESS_REG          0x0002

// Resolutions according to manual
#define PZEM_VOLTAGE_RESOLUTION    0.1f
#define PZEM_CURRENT_RESOLUTION    0.001f
#define PZEM_POWER_RESOLUTION      0.1f
#define PZEM_ENERGY_RESOLUTION     1.0f
#define PZEM_FREQUENCY_RESOLUTION  0.1f
#define PZEM_POWER_FACTOR_RESOLUTION 0.01f

class PZEM004T : public RS485 {
public:
    // Constructor
    PZEM004T(Stream &serial, uint8_t slaveAddr = 0xF8);
    
    // Measurement methods
    float readVoltage();
    float readCurrent();
    float readPower();
    float readEnergy();
    float readFrequency();
    float readPowerFactor();
    bool readAlarmStatus();
    
    // Method to read all measurements at once
    bool readAll(float* voltage, float* current, float* power, 
                 float* energy, float* frequency, float* powerFactor, bool* alarm);
    
    // Parameter methods
    bool setAlarmThreshold(uint16_t threshold);
    bool setAddress(uint8_t newAddress);
    uint16_t getAlarmThreshold();
    uint8_t getAddress();
    
    // Control methods
    bool resetEnergy();

private:
    uint8_t _slaveAddr;
    
    // Internal methods
    uint32_t combineRegisters(uint16_t low, uint16_t high);
};

#endif // PZEM004T_H