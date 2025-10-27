#ifndef PZEM6L24_H
#define PZEM6L24_H

#include "RS485.h"

// PZEM-6L24 register addresses
#define PZEM_VOLTAGE_REG            0x0000
#define PZEM_CURRENT_REG            0x0003
#define PZEM_FREQUENCY_REG          0x0006
#define PZEM_VOLTAGE_PHASE_REG      0x0009
#define PZEM_CURRENT_PHASE_REG      0x000B
#define PZEM_ACTIVE_POWER_REG       0x000E
#define PZEM_REACTIVE_POWER_REG     0x0014
#define PZEM_APPARENT_POWER_REG     0x001A
#define PZEM_ACTIVE_POWER_COMBINED_REG   0x0020
#define PZEM_REACTIVE_POWER_COMBINED_REG 0x0022
#define PZEM_APPARENT_POWER_COMBINED_REG 0x0024
#define PZEM_POWER_FACTOR_A_B_REG   0x0026
#define PZEM_POWER_FACTOR_C_COMBINED_REG 0x0027
#define PZEM_ACTIVE_ENERGY_REG      0x0028
#define PZEM_REACTIVE_ENERGY_REG    0x002E
#define PZEM_APPARENT_ENERGY_REG    0x0034
#define PZEM_ACTIVE_ENERGY_COMBINED_REG   0x003A
#define PZEM_REACTIVE_ENERGY_COMBINED_REG 0x003C
#define PZEM_APPARENT_ENERGY_COMBINED_REG 0x003E

// Parameter registers
#define PZEM_ADDRESS_REG          0x0000
#define PZEM_BAUDRATE_TYPE_REG    0x0001
#define PZEM_FREQUENCY_SYSTEM_REG   0x0002

// Resolutions according to manual
#define PZEM_VOLTAGE_RESOLUTION    0.1f
#define PZEM_CURRENT_RESOLUTION    0.01f
#define PZEM_FREQUENCY_RESOLUTION  0.01f
#define PZEM_POWER_RESOLUTION      0.1f
#define PZEM_POWER_FACTOR_RESOLUTION 0.01f
#define PZEM_ENERGY_RESOLUTION     0.1f
#define PZEM_PHASE_RESOLUTION     0.01f

// Reset energy options
#define PZEM_RESET_ENERGY_A     0x00
#define PZEM_RESET_ENERGY_B     0x01
#define PZEM_RESET_ENERGY_C     0x02
#define PZEM_RESET_ENERGY_COMBINED     0x03
#define PZEM_RESET_ENERGY_ALL     0x0F

// Baudrate options
#define PZEM_BAUDRATE_2400      0x00
#define PZEM_BAUDRATE_4800      0x01
#define PZEM_BAUDRATE_9600      0x02
#define PZEM_BAUDRATE_19200     0x03
#define PZEM_BAUDRATE_38400     0x04
#define PZEM_BAUDRATE_57600     0x05
#define PZEM_BAUDRATE_115200    0x06

// Connection type options
#define PZEM_CONNECTION_3PHASE_4WIRE    0x00
#define PZEM_CONNECTION_3PHASE_3WIRE   0x01

// Frequency options
#define PZEM_FREQUENCY_50HZ            0x00
#define PZEM_FREQUENCY_60HZ            0x01

class PZEM6L24 : public RS485 {
public:
    // Constructors
#if defined(__AVR_ATmega328P__) 
    PZEM6L24(SoftwareSerial &serial, uint8_t slaveAddr = 0xF8);
#else
    PZEM6L24(HardwareSerial &serial, uint8_t slaveAddr = 0xF8);
    PZEM6L24(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr = 0xF8);
#endif

    void begin(uint32_t baudrate = 9600);
    
    // Basic measurements by phase (0=A, 1=B, 2=C)
    float readVoltage(uint8_t phase);
    float readCurrent(uint8_t phase);
    float readFrequency(uint8_t phase);
    float readActivePower(uint8_t phase);
    float readReactivePower(uint8_t phase);
    float readApparentPower(uint8_t phase);
    float readPowerFactor(uint8_t phase);
    float readActiveEnergy(uint8_t phase);
    float readReactiveEnergy(uint8_t phase);
    float readApparentEnergy(uint8_t phase);
    float readVoltagePhaseAngle(uint8_t phase);
    float readCurrentPhaseAngle(uint8_t phase);
    
    // Combined measurements
    float readActivePower();
    float readReactivePower();
    float readApparentPower();
    float readPowerFactor();
    float readActiveEnergy();
    float readReactiveEnergy();
    float readApparentEnergy();
    
    // Multi-phase reading methods
    void readVoltage(float& voltageA, float& voltageB, float& voltageC);
    void readCurrent(float& currentA, float& currentB, float& currentC);
    void readFrequency(float& frequencyA, float& frequencyB, float& frequencyC);
    void readActivePower(float& powerA, float& powerB, float& powerC);
    void readReactivePower(float& powerA, float& powerB, float& powerC);
    void readApparentPower(float& powerA, float& powerB, float& powerC);
    void readPowerFactor(float& factorA, float& factorB, float& factorC);
    void readActiveEnergy(float& energyA, float& energyB, float& energyC);
    void readReactiveEnergy(float& energyA, float& energyB, float& energyC);
    void readApparentEnergy(float& energyA, float& energyB, float& energyC);
    void readVoltagePhaseAngle(float& angleA, float& angleB, float& angleC);
    void readCurrentPhaseAngle(float& angleA, float& angleB, float& angleC);
    
    // Parameter methods
    bool setAddress(uint8_t address = 0x00);
    bool setBaudrate(uint8_t baudrate = PZEM_BAUDRATE_9600);
    bool setBaudrateAndConnectionType(uint8_t baudrate = PZEM_BAUDRATE_9600, uint8_t connectionType = PZEM_CONNECTION_3PHASE_4WIRE);
    bool setFrequency(uint8_t frequencyType = PZEM_FREQUENCY_50HZ);
    bool getSoftwareHardwareSettings();
    uint8_t getAddress();
    uint8_t getBaudrate();
    uint8_t getConnectionType();
    uint8_t getFrequency();
    
    // Control methods
    bool resetEnergy(uint8_t phaseOption = PZEM_RESET_ENERGY_ALL);

private:
    uint8_t _slaveAddr;
#if !defined(__AVR_ATmega328P__)
    uint8_t _rxPin;
    uint8_t _txPin;
#endif
};

#endif // PZEM6L24_H