/**
 * @file PZEM6L24.h
 * @brief PZEM-6L24 three-phase energy monitoring module class
 * @author Lucas Hudson
 * @date 2025
 */

#ifndef PZEM6L24_H
#define PZEM6L24_H

#include "RS485.h"

/**
 * @defgroup PZEM6L24Registers PZEM-6L24 Register Addresses
 * @brief Input register addresses for three-phase measurements
 * @{
 */
// Input register addresses
#define PZEM_VOLTAGE_REG                  0x0000 // A=0x0000, B=0x0001, C=0x0002
#define PZEM_CURRENT_REG                  0x0003 // A=0x0003, B=0x0004, C=0x0005
#define PZEM_FREQUENCY_REG                0x0006 // A=0x0006, B=0x0007, C=0x0008
#define PZEM_VOLTAGE_PHASE_REG            0x0009 // B=0x0009, C=0x000A (A is reference)
#define PZEM_CURRENT_PHASE_REG            0x000B // A=0x000B, B=0x000C, C=0x000D
#define PZEM_ACTIVE_POWER_REG             0x000E // A=0x000E/0x000F, B=0x0010/0x0011, C=0x0012/0x0013
#define PZEM_REACTIVE_POWER_REG           0x0014 // A=0x0014/0x0015, B=0x0016/0x0017, C=0x0018/0x0019
#define PZEM_APPARENT_POWER_REG           0x001A // A=0x001A/0x001B, B=0x001C/0x001D, C=0x001E/0x001F
#define PZEM_ACTIVE_POWER_COMBINED_REG    0x0020 // combined=0x0020/0x0021
#define PZEM_REACTIVE_POWER_COMBINED_REG  0x0022 // combined=0x0022/0x0023
#define PZEM_APPARENT_POWER_COMBINED_REG  0x0024 // combined=0x0024/0x0025
#define PZEM_POWER_FACTOR_A_B_REG         0x0026 // A=hi, B=lo
#define PZEM_POWER_FACTOR_C_COMBINED_REG  0x0027 // C=hi, combined=lo
#define PZEM_ACTIVE_ENERGY_REG            0x0028 // A=0x0028/0x0029, B=0x002A/0x002B, C=0x002C/0x002D
#define PZEM_REACTIVE_ENERGY_REG          0x002E // A=0x002E/0x002F, B=0x0030/0x0031, C=0x0032/0x0033
#define PZEM_APPARENT_ENERGY_REG          0x0034 // A=0x0034/0x0035, B=0x0036/0x0037, C=0x0038/0x0039
#define PZEM_ACTIVE_ENERGY_COMBINED_REG   0x003A // combined=0x003A/0x003B
#define PZEM_REACTIVE_ENERGY_COMBINED_REG 0x003C // combined=0x003C/0x003D
#define PZEM_APPARENT_ENERGY_COMBINED_REG 0x003E ///< Apparent energy combined register address

// Parameter registers
#define PZEM_ADDRESS_REG            0x0000  ///< Address register (addr=hi, addr type=lo)
#define PZEM_BAUDRATE_TYPE_REG      0x0001  ///< Baudrate/connection type register (connection type=hi, baudrate=lo)
#define PZEM_FREQUENCY_SYSTEM_REG   0x0002  ///< Frequency system register (reserved=hi, frequency=lo)
/** @} */

/**
 * @defgroup PZEM6L24Resolutions PZEM-6L24 Resolutions
 * @brief Resolution factors for converting raw register values to physical units
 * @{
 */
#define PZEM_VOLTAGE_RESOLUTION      0.1f
#define PZEM_CURRENT_RESOLUTION      0.01f
#define PZEM_FREQUENCY_RESOLUTION    0.01f
#define PZEM_POWER_RESOLUTION        0.1f
#define PZEM_POWER_FACTOR_RESOLUTION 0.01f
#define PZEM_ENERGY_RESOLUTION       0.1f
#define PZEM_PHASE_RESOLUTION        0.01f  ///< Phase angle resolution (degrees per LSB)
/** @} */

/**
 * @defgroup PZEM6L24ResetOptions Reset Energy Options
 * @brief Options for selective energy counter reset
 * @{
 */
#define PZEM_RESET_ENERGY_A        0x00  ///< Reset phase A energy
#define PZEM_RESET_ENERGY_B        0x01  ///< Reset phase B energy
#define PZEM_RESET_ENERGY_C        0x02  ///< Reset phase C energy
#define PZEM_RESET_ENERGY_COMBINED 0x03  ///< Reset combined energy
#define PZEM_RESET_ENERGY_ALL      0x0F  ///< Reset all energy counters
/** @} */

/**
 * @defgroup PZEM6L24Baudrates Baudrate Options
 * @brief Supported baudrate values
 * @{
 */
#define PZEM_BAUDRATE_2400   0x00
#define PZEM_BAUDRATE_4800   0x01
#define PZEM_BAUDRATE_9600   0x02
#define PZEM_BAUDRATE_19200  0x03
#define PZEM_BAUDRATE_38400  0x04
#define PZEM_BAUDRATE_57600  0x05
#define PZEM_BAUDRATE_115200 0x06  ///< 115200 baud
/** @} */

/**
 * @defgroup PZEM6L24ConnectionTypes Connection Type Options
 * @brief Three-phase connection type options
 * @{
 */
#define PZEM_CONNECTION_3PHASE_4WIRE 0x00  ///< 3-phase 4-wire connection
#define PZEM_CONNECTION_3PHASE_3WIRE 0x01  ///< 3-phase 3-wire connection
/** @} */

/**
 * @defgroup PZEM6L24Frequencies Frequency Options
 * @brief AC frequency system options
 * @{
 */
#define PZEM_FREQUENCY_50HZ 0x00  ///< 50 Hz AC system
#define PZEM_FREQUENCY_60HZ 0x01  ///< 60 Hz AC system
/** @} */

/**
 * @class PZEM6L24
 * @brief Class for interfacing with PZEM-6L24 three-phase energy monitoring module
 * 
 * This class provides comprehensive methods to read three-phase electrical measurements
 * including voltage, current, power (active, reactive, apparent), energy, power factor,
 * and phase angles for each phase (A, B, C) as well as combined values.
 */
class PZEM6L24 : public RS485 {
public:
    /**
     * @name Constructors
     * @{
     */
    
    /**
     * @brief Constructor for AVR ATmega328P (Arduino Uno/Nano) with SoftwareSerial
     * @param serial SoftwareSerial object reference
     * @param slaveAddr Slave device address (default: 0xF8)
     */
#if defined(__AVR_ATmega328P__) 
    PZEM6L24(SoftwareSerial &serial, uint8_t slaveAddr = 0xF8);
#else
    /**
     * @brief Constructor for ESP32/ESP8266 with HardwareSerial
     * @param serial HardwareSerial object reference
     * @param slaveAddr Slave device address (default: 0xF8)
     */
    PZEM6L24(HardwareSerial &serial, uint8_t slaveAddr = 0xF8);
    
    /**
     * @brief Constructor for ESP32/ESP8266 with HardwareSerial and custom pins
     * @param serial HardwareSerial object reference
     * @param rxPin RX pin number
     * @param txPin TX pin number
     * @param slaveAddr Slave device address (default: 0xF8)
     */
    PZEM6L24(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr = 0xF8);
#endif
    
    /** @} */
    
    /**
     * @brief Initialize serial communication
     * @param baudrate Serial baudrate (default: 9600)
     */
    void begin(uint32_t baudrate = 9600);
    
    /**
     * @name Basic Phase Measurements
     * @brief Read measurements for a specific phase (0=A, 1=B, 2=C)
     * @{
     */
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
    
    /** @} */
    
    /**
     * @name Combined Measurements
     * @brief Read combined three-phase measurements
     * @{
     */
    float readActivePower();
    float readReactivePower();
    float readApparentPower();
    float readPowerFactor();
    float readActiveEnergy();
    float readReactiveEnergy();
    float readApparentEnergy();
    
    /** @} */
    
    /**
     * @name Multi-Phase Reading Methods
     * @brief Read all three phases simultaneously
     * @{
     */
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
    
    /** @} */
    
    /**
     * @name Parameter Methods
     * @brief Configuration and parameter management
     * @{
     */
    bool setAddress(uint8_t address = 0x00);
    bool setBaudrateAndConnectionType(uint8_t baudrate = PZEM_BAUDRATE_9600, uint8_t connectionType = PZEM_CONNECTION_3PHASE_4WIRE, bool forceBaudrate = true);
    bool setFrequency(uint8_t frequencyType = PZEM_FREQUENCY_50HZ);
    bool getSoftwareHardwareSettings();
    uint8_t getAddress();
    uint32_t getBaudrate();
    uint8_t getConnectionType();
    uint8_t getFrequency();
    
    /** @} */
    
    /**
     * @name Control Methods
     * @{
     */
    
    /**
     * @brief Reset energy counter(s)
     * @param phaseOption Reset option (PZEM_RESET_ENERGY_A, PZEM_RESET_ENERGY_B, PZEM_RESET_ENERGY_C, PZEM_RESET_ENERGY_COMBINED, or PZEM_RESET_ENERGY_ALL)
     * @return true if successful, false otherwise
     */
    bool resetEnergy(uint8_t phaseOption = PZEM_RESET_ENERGY_ALL);

    /** @} */

private:
    uint8_t _slaveAddr;  ///< Current slave device address
#if !defined(__AVR_ATmega328P__)
    uint8_t _rxPin;      ///< RX pin number (-1 if not used)
    uint8_t _txPin;      ///< TX pin number (-1 if not used)
#endif
};

#endif // PZEM6L24_H