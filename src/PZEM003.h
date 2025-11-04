/**
 * @file PZEM003.h
 * @brief PZEM-003 energy monitoring module class
 * @author Lucas Hudson
 * @date 2025
 */

#ifndef PZEM003_H
#define PZEM003_H

#include "RS485.h"

/**
 * @defgroup PZEM003Registers PZEM-003 Register Addresses
 * @brief Register addresses for measurement and parameter registers
 * @{
 */
#define PZEM_VOLTAGE_REG              0x0000  ///< Voltage register address
#define PZEM_CURRENT_REG              0x0001  ///< Current register address
#define PZEM_POWER_LOW_REG            0x0002  ///< Power low register address
#define PZEM_ENERGY_LOW_REG           0x0004  ///< Energy low register address
#define PZEM_HIGH_VOLTAGE_ALARM_REG   0x0006  ///< High voltage alarm register address
#define PZEM_LOW_VOLTAGE_ALARM_REG    0x0007  ///< Low voltage alarm register address

// Parameter registers
#define PZEM_HIGH_VOLTAGE_THRESHOLD_REG  0x0000  ///< High voltage threshold register address
#define PZEM_LOW_VOLTAGE_THRESHOLD_REG   0x0001  ///< Low voltage threshold register address
#define PZEM_ADDRESS_REG                 0x0002  ///< Device address register address
/** @} */

/**
 * @defgroup PZEM003Resolutions PZEM-003 Resolutions
 * @brief Resolution factors for converting raw register values to physical units
 * @{
 */
#define PZEM_VOLTAGE_RESOLUTION            0.01f  ///< Voltage resolution (V per LSB)
#define PZEM_HIGH_VOLTAGE_ALARM_RESOLUTION 0.01f  ///< High voltage alarm threshold resolution (V per LSB)
#define PZEM_LOW_VOLTAGE_ALARM_RESOLUTION  0.01f  ///< Low voltage alarm threshold resolution (V per LSB)
#define PZEM_CURRENT_RESOLUTION            0.01f  ///< Current resolution (A per LSB)
#define PZEM_POWER_RESOLUTION              0.1f   ///< Power resolution (W per LSB)
#define PZEM_ENERGY_RESOLUTION             1.0f  ///< Energy resolution (Wh per LSB)
/** @} */

/**
 * @class PZEM003
 * @brief Class for interfacing with PZEM-003 single-phase energy monitoring modules
 * 
 * This class provides methods to read voltage, current, power, and energy from
 * PZEM-003 devices via Modbus-RTU protocol.
 */
class PZEM003 : public RS485 {
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
    PZEM003(SoftwareSerial &serial, uint8_t slaveAddr = 0xF8);
#else
    /**
     * @brief Constructor for ESP32/ESP8266 with HardwareSerial
     * @param serial HardwareSerial object reference
     * @param slaveAddr Slave device address (default: 0xF8)
     */
    PZEM003(HardwareSerial &serial, uint8_t slaveAddr = 0xF8);
    
    /**
     * @brief Constructor for ESP32/ESP8266 with HardwareSerial and custom pins
     * @param serial HardwareSerial object reference
     * @param rxPin RX pin number
     * @param txPin TX pin number
     * @param slaveAddr Slave device address (default: 0xF8)
     */
    PZEM003(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr = 0xF8);
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
     * @brief Read high voltage alarm status
     * @return true if alarm is active, false otherwise
     */
    bool readHighVoltageAlarm();
    
    /**
     * @brief Read low voltage alarm status
     * @return true if alarm is active, false otherwise
     */
    bool readLowVoltageAlarm();
    
    /**
     * @brief Read all measurements at once
     * @param voltage Pointer to store voltage value
     * @param current Pointer to store current value
     * @param power Pointer to store power value
     * @param energy Pointer to store energy value
     * @return true if successful, false otherwise
     */
    bool readAll(float* voltage, float* current, float* power, float* energy);
    
    /** @} */
    
    /**
     * @name Parameter Methods
     * @{
     */
    
    /**
     * @brief Set high voltage alarm threshold
     * @param threshold Voltage threshold in volts
     * @return true if successful, false otherwise
     */
    bool setHighVoltageAlarm(float threshold = 300.0);
    
    /**
     * @brief Set low voltage alarm threshold
     * @param threshold Voltage threshold in volts
     * @return true if successful, false otherwise
     */
    bool setLowVoltageAlarm(float threshold = 7.0);
    
    /**
     * @brief Set device slave address
     * @param newAddress New slave address (0x01 to 0xF7)
     * @return true if successful, false otherwise
     */
    bool setAddress(uint8_t newAddress = 0x01);

    /**
     * @brief Get high voltage alarm threshold
     * @return Voltage threshold in volts, or NAN on error
     */
    float getHighVoltageAlarm();
    
    /**
     * @brief Get low voltage alarm threshold
     * @return Voltage threshold in volts, or NAN on error
     */
    float getLowVoltageAlarm();
    
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

protected:
    uint8_t _slaveAddr;  ///< Current slave device address
#if !defined(__AVR_ATmega328P__)
    uint8_t _rxPin;      ///< RX pin number (-1 if not used)
    uint8_t _txPin;      ///< TX pin number (-1 if not used)
#endif

};

#endif // PZEM003_H
