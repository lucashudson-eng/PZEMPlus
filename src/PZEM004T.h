/**
 * @file PZEM004T.h
 * @brief PZEM-004T energy monitoring module class
 * @author Lucas Hudson
 * @date 2025
 */

#ifndef PZEM004T_H
#define PZEM004T_H

#include "RS485.h"

/**
 * @defgroup PZEM004TRegisters PZEM-004T Register Addresses
 * @brief Register addresses for PZEM-004T measurement and parameter registers
 * @{
 */
#define PZEM_VOLTAGE_REG         0x0000  ///< Voltage register address
#define PZEM_CURRENT_LOW_REG      0x0001  ///< Current low register address
#define PZEM_POWER_LOW_REG        0x0003  ///< Power low register address
#define PZEM_ENERGY_LOW_REG       0x0005  ///< Energy low register address
#define PZEM_FREQUENCY_REG        0x0007  ///< Frequency register address
#define PZEM_POWER_FACTOR_REG     0x0008  ///< Power factor register address
#define PZEM_POWER_ALARM_REG     0x0009  ///< Power alarm status register address

// Parameter registers
#define PZEM_POWER_THRESHOLD_REG  0x0001  ///< Power alarm threshold register address
#define PZEM_ADDRESS_REG          0x0002  ///< Device address register address
/** @} */

/**
 * @defgroup PZEM004TResolutions PZEM-004T Resolutions
 * @brief Resolution factors for converting raw register values to physical units
 * @{
 */
#define PZEM_VOLTAGE_RESOLUTION    0.1f    ///< Voltage resolution (V per LSB)
#define PZEM_CURRENT_RESOLUTION    0.001f  ///< Current resolution (A per LSB)
#define PZEM_POWER_RESOLUTION      0.1f    ///< Power resolution (W per LSB)
#define PZEM_POWER_ALARM_RESOLUTION 1.0f   ///< Power alarm threshold resolution (W per LSB)
#define PZEM_ENERGY_RESOLUTION     1.0f    ///< Energy resolution (Wh per LSB)
#define PZEM_FREQUENCY_RESOLUTION  0.1f    ///< Frequency resolution (Hz per LSB)
#define PZEM_POWER_FACTOR_RESOLUTION 0.01f ///< Power factor resolution (per LSB)
/** @} */

/**
 * @class PZEM004T
 * @brief Class for interfacing with PZEM-004T single-phase energy monitoring module
 * 
 * This class provides methods to read voltage, current, power, energy,
 * frequency, and power factor from PZEM-004T devices via Modbus-RTU protocol.
 * It also supports configuring alarm thresholds and device address.
 */
class PZEM004T : public RS485 {
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
    PZEM004T(SoftwareSerial &serial, uint8_t slaveAddr = 0xF8);
#else
    /**
     * @brief Constructor for ESP32/ESP8266 with HardwareSerial
     * @param serial HardwareSerial object reference
     * @param slaveAddr Slave device address (default: 0xF8)
     */
    PZEM004T(HardwareSerial &serial, uint8_t slaveAddr = 0xF8);
    
    /**
     * @brief Constructor for ESP32/ESP8266 with HardwareSerial and custom pins
     * @param serial HardwareSerial object reference
     * @param rxPin RX pin number
     * @param txPin TX pin number
     * @param slaveAddr Slave device address (default: 0xF8)
     */
    PZEM004T(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr = 0xF8);
#endif
    
    /** @} */
    
    /**
     * @brief Initialize serial communication
     * @param baudrate Serial baudrate (default: 9600)
     */
    void begin(uint32_t baudrate = 9600);
    
    /**
     * @name Measurement Methods
     * @{
     */
    
    /**
     * @brief Read voltage from device
     * @return Voltage in volts, or NAN on error
     */
    float readVoltage();
    
    /**
     * @brief Read current from device
     * @return Current in amperes, or NAN on error
     */
    float readCurrent();
    
    /**
     * @brief Read power from device
     * @return Power in watts, or NAN on error
     */
    float readPower();
    
    /**
     * @brief Read energy from device
     * @return Energy in watt-hours, or NAN on error
     */
    float readEnergy();
    
    /**
     * @brief Read frequency from device
     * @return Frequency in hertz, or NAN on error
     */
    float readFrequency();
    
    /**
     * @brief Read power factor from device
     * @return Power factor (0.00 to 1.00), or NAN on error
     */
    float readPowerFactor();
    
    /**
     * @brief Read power alarm status from device
     * @return true if alarm is active, false otherwise
     */
    bool readPowerAlarm();
    
    /**
     * @brief Read all measurements at once
     * @param voltage Pointer to store voltage value
     * @param current Pointer to store current value
     * @param power Pointer to store power value
     * @param energy Pointer to store energy value
     * @param frequency Pointer to store frequency value
     * @param powerFactor Pointer to store power factor value
     * @return true if successful, false otherwise
     */
    bool readAll(float* voltage, float* current, float* power, float* energy, float* frequency, float* powerFactor);
    
    /** @} */
    
    /**
     * @name Parameter Methods
     * @{
     */
    
    /**
     * @brief Set power alarm threshold
     * @param threshold Power threshold in watts
     * @return true if successful, false otherwise
     */
    bool setPowerAlarm(float threshold = 2300.0);
    
    /**
     * @brief Set device slave address
     * @param newAddress New slave address (0x01 to 0xF7)
     * @return true if successful, false otherwise
     */
    bool setAddress(uint8_t newAddress = 0x01);
    
    /**
     * @brief Get power alarm threshold
     * @return Power threshold in watts, or NAN on error
     */
    float getPowerAlarm();
    
    /**
     * @brief Get device slave address
     * @return Current slave address
     */
    uint8_t getAddress();
    
    /** @} */
    
    /**
     * @name Control Methods
     * @{
     */
    
    /**
     * @brief Reset energy counter
     * @return true if successful, false otherwise
     */
    bool resetEnergy();

    /** @} */

private:
    uint8_t _slaveAddr;  ///< Current slave device address
#if !defined(__AVR_ATmega328P__)
    uint8_t _rxPin;      ///< RX pin number (-1 if not used)
    uint8_t _txPin;      ///< TX pin number (-1 if not used)
#endif
    
};

#endif // PZEM004T_H
