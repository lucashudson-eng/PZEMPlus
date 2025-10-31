/**
 * @file RS485.h
 * @brief RS485 communication class for Modbus-RTU protocol
 * @author Lucas Hudson
 * @date 2025
 */

#ifndef RS485_H
#define RS485_H

#include <Arduino.h>
#if defined(__AVR_ATmega328P__)
#include <SoftwareSerial.h>
#endif

/**
 * @defgroup ModbusFunctionCodes Modbus-RTU Function Codes
 * @brief Modbus-RTU protocol function codes
 * @{
 */
#define MODBUS_READ_HOLDING_REGISTERS   0x03  ///< Read holding registers function code
#define MODBUS_READ_INPUT_REGISTERS     0x04  ///< Read input registers function code
#define MODBUS_WRITE_SINGLE_REGISTER    0x06  ///< Write single register function code
#define MODBUS_WRITE_MULTIPLE_REGISTERS 0x10  ///< Write multiple registers function code
#define MODBUS_RESET_ENERGY             0x42  ///< Reset energy counter function code
/** @} */

/**
 * @class RS485
 * @brief Base class for RS485 communication using Modbus-RTU protocol
 * 
 * This class provides low-level Modbus-RTU communication functions including
 * reading/writing registers, CRC calculation, and RS485 enable pin control.
 * It serves as the base class for all PZEM device implementations.
 */
class RS485 {
public:
    /**
     * @brief Constructor for RS485 communication class
     * @param serial Pointer to Stream object (HardwareSerial or SoftwareSerial)
     */
    RS485(Stream* serial);
    
    /**
     * @name Generic Communication Methods
     * @{
     */
    
    /**
     * @brief Read holding registers from Modbus device (function code 0x03)
     * @param slaveAddr Slave device address
     * @param startAddr Starting register address
     * @param numRegs Number of registers to read
     * @param data Pointer to data buffer for storing read values
     * @param big_endian Byte order flag (true = big endian, false = little endian)
     * @return true if successful, false otherwise
     */
    bool readHoldingRegisters(uint8_t slaveAddr, uint16_t startAddr, uint16_t numRegs, uint16_t* data, bool big_endian = true);
    
    /**
     * @brief Read input registers from Modbus device (function code 0x04)
     * @param slaveAddr Slave device address
     * @param startAddr Starting register address
     * @param numRegs Number of registers to read
     * @param data Pointer to data buffer for storing read values
     * @param big_endian Byte order flag (true = big endian, false = little endian)
     * @return true if successful, false otherwise
     */
    bool readInputRegisters(uint8_t slaveAddr, uint16_t startAddr, uint16_t numRegs, uint16_t* data, bool big_endian = true);
    
    /**
     * @brief Write single register to Modbus device (function code 0x06)
     * @param slaveAddr Slave device address
     * @param regAddr Register address to write
     * @param value Value to write
     * @param big_endian Byte order flag (true = big endian, false = little endian)
     * @return true if successful, false otherwise
     */
    bool writeSingleRegister(uint8_t slaveAddr, uint16_t regAddr, uint16_t value, bool big_endian = true);
    
    /**
     * @brief Write multiple registers to Modbus device (function code 0x10)
     * @param slaveAddr Slave device address
     * @param startAddr Starting register address
     * @param numRegs Number of registers to write
     * @param data Pointer to data buffer containing values to write
     * @param big_endian Byte order flag (true = big endian, false = little endian)
     * @return true if successful, false otherwise
     */
    bool writeMultipleRegisters(uint8_t slaveAddr, uint16_t startAddr, uint16_t numRegs, uint16_t* data, bool big_endian = true);
    
    /**
     * @brief Reset energy counter (function code 0x42)
     * @param slaveAddr Slave device address
     * @return true if successful, false otherwise
     */
    bool resetEnergy(uint8_t slaveAddr);
    
    /**
     * @brief Reset energy counter with phase selection (function code 0x42)
     * @param slaveAddr Slave device address
     * @param phaseSequence Phase sequence byte for selective reset (for PZEM-6L24)
     * @return true if successful, false otherwise
     */
    bool resetEnergy(uint8_t slaveAddr, uint8_t phaseSequence);
    
    /** @} */
    
    /**
     * @name Utility Methods
     * @{
     */
    
    /**
     * @brief Calculate Modbus CRC16 checksum
     * @param data Pointer to data buffer
     * @param length Length of data buffer
     * @return Calculated CRC16 value
     */
    uint16_t calculateCRC16(uint8_t* data, uint8_t length);
    
    /**
     * @brief Verify Modbus CRC16 checksum in received data
     * @param data Pointer to data buffer (including CRC bytes)
     * @param length Total length of data buffer including CRC
     * @return true if CRC is valid, false otherwise
     */
    bool verifyCRC16(uint8_t* data, uint8_t length);
    
    /**
     * @brief Set communication timeout
     * @param responseTimeout Timeout value in milliseconds
     */
    void setTimeouts(uint32_t responseTimeout);
    
    /**
     * @brief Combine two 16-bit registers into a 32-bit value
     * @param low Low 16-bit register value
     * @param high High 16-bit register value
     * @param signed_result If true, result is interpreted as signed integer
     * @return Combined 32-bit value
     */
    uint32_t combineRegisters(uint16_t low, uint16_t high, bool signed_result = false);
    
    /**
     * @brief Clear serial buffer
     */
    void clearBuffer();
    
    /** @} */
    
    /**
     * @name Configuration Methods
     * @{
     */
    
    /**
     * @brief Set RS485 enable pin for MAX485 transceiver
     * @param enablePin GPIO pin number for DE/RE control
     * @return true if successful, false otherwise
     */
    bool setEnable(uint8_t enablePin);
    
    /**
     * @brief Get serial stream pointer
     * @return Pointer to Stream object
     */
    Stream* getSerial();
    
    /** @} */

private:
    Stream* _serial;        ///< Pointer to serial communication stream
    uint32_t _responseTimeout;  ///< Response timeout in milliseconds
    uint8_t _rs485_en;      ///< RS485 enable pin number (-1 if not used)
    
    /**
     * @name Internal Methods
     * @{
     */
    
    /**
     * @brief Enable RS485 transmit mode (DE/RE = HIGH)
     */
    void enableTransmit();
    
    /**
     * @brief Enable RS485 receive mode (DE/RE = LOW)
     */
    void enableReceive();
    
    /** @} */
};

#endif // RS485_H