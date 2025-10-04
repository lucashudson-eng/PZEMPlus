#ifndef RS485_H
#define RS485_H

#include <Arduino.h>

// Modbus-RTU function codes
#define MODBUS_READ_HOLDING_REGISTERS   0x03
#define MODBUS_READ_INPUT_REGISTERS     0x04
#define MODBUS_WRITE_SINGLE_REGISTER    0x06
#define MODBUS_RESET_ENERGY             0x42

// Modbus error codes
#define MODBUS_ERROR_ILLEGAL_FUNCTION    0x01
#define MODBUS_ERROR_ILLEGAL_ADDRESS     0x02
#define MODBUS_ERROR_ILLEGAL_DATA        0x03
#define MODBUS_ERROR_SLAVE_ERROR         0x04

// Special addresses
#define MODBUS_BROADCAST_ADDRESS         0x00
#define MODBUS_GENERAL_ADDRESS           0xF8

// Define for default configurations
#define TIMEOUT 100

class RS485 {
public:
    // Constructor
    RS485(Stream* serial);
    
    // Generic communication methods
    bool readHoldingRegisters(uint8_t slaveAddr, uint16_t startAddr, uint16_t numRegs, uint16_t* data);
    bool readInputRegisters(uint8_t slaveAddr, uint16_t startAddr, uint16_t numRegs, uint16_t* data);
    bool writeSingleRegister(uint8_t slaveAddr, uint16_t regAddr, uint16_t value);
    bool resetEnergy(uint8_t slaveAddr);
    
    // Utility methods
    uint16_t calculateCRC16(uint8_t* data, uint8_t length);
    bool verifyCRC16(uint8_t* data, uint8_t length);
    void setTimeouts(uint32_t responseTimeout = 1000);
    
    // Configuration methods
    void setDebug(bool enable);

private:
    Stream* _serial;
    uint32_t _responseTimeout;
    bool _debug;
    
    // Internal methods
    void debugPrint(const char* message);
    void debugPrintHex(uint8_t* data, uint8_t length);
};

#endif // RS485_H