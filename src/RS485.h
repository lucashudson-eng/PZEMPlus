#ifndef RS485_H
#define RS485_H

#include <Arduino.h>
#if defined(__AVR_ATmega328P__)
#include <SoftwareSerial.h>
#endif

// Modbus-RTU function codes
#define MODBUS_READ_HOLDING_REGISTERS   0x03
#define MODBUS_READ_INPUT_REGISTERS     0x04
#define MODBUS_WRITE_SINGLE_REGISTER    0x06
#define MODBUS_WRITE_MULTIPLE_REGISTERS 0x10
#define MODBUS_RESET_ENERGY             0x42

class RS485 {
public:
    // Constructor
    RS485(Stream* serial);

    enum class RegByteOrder { HighByteFirst, HighByteSecond };
    static constexpr RegByteOrder kDefaultOrder =
    #if defined(DEVICE_PZEM_6L24) || defined(PZEM_6L24)
        RegByteOrder::HighByteSecond;
    #else
        RegByteOrder::HighByteFirst;
    #endif
        
    bool readInputRegisters(uint8_t slaveAddr,uint16_t startAddr,uint16_t numRegs,uint16_t* data); // Legacy overload (no default args needed here)
    bool readInputRegisters(uint8_t slaveAddr,uint16_t startAddr,uint16_t numRegs,uint16_t* data,RegByteOrder order);  // New overload which allow the specification of register byte order (this is different for PZEM-6L24)

    // Generic communication methods
    bool readHoldingRegisters(uint8_t slaveAddr, uint16_t startAddr, uint16_t numRegs, uint16_t* data);
    bool writeMultipleRegisters(uint8_t slaveAddr, uint16_t startAddr, uint16_t numRegs, uint16_t* data);
    bool writeSingleRegister(uint8_t slaveAddr, uint16_t regAddr, uint16_t value);
    bool resetEnergy(uint8_t slaveAddr);
    bool resetEnergy(uint8_t slaveAddr, uint8_t phaseSequence);
    
    // Utility methods
    uint16_t calculateCRC16(uint8_t* data, uint8_t length);
    bool verifyCRC16(uint8_t* data, uint8_t length);
    void setTimeouts(uint32_t responseTimeout);
    uint32_t combineRegisters(uint16_t low, uint16_t high);
    void clearBuffer();
    
    // Configuration methods
    bool setEnable(uint8_t enablePin);
    Stream* getSerial();

private:
    Stream* _serial;
    uint32_t _responseTimeout;
    int8_t _rs485_en; 
    
    // Internal methods
    void enableTransmit();
    void enableReceive();
};

#endif // RS485_H