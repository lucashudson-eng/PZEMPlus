#include "RS485.h"

// Constructor
RS485::RS485(Stream* serial)
    : _serial(serial), _responseTimeout(100), _rs485_en(-1) {
}

// Method to read holding registers (function 0x03)
bool RS485::readHoldingRegisters(uint8_t slaveAddr, uint16_t startAddr, uint16_t numRegs, uint16_t* data, bool big_endian) {
    
    uint8_t request[8];
    request[0] = slaveAddr;
    request[1] = MODBUS_READ_HOLDING_REGISTERS;
    request[2] = (startAddr >> 8) & 0xFF;  // High byte
    request[3] = startAddr & 0xFF;         // Low byte
    request[4] = (numRegs >> 8) & 0xFF;    // High byte
    request[5] = numRegs & 0xFF;           // Low byte
    
    uint16_t crc = calculateCRC16(request, 6);
    request[6] = crc & 0xFF;               // CRC Low byte
    request[7] = (crc >> 8) & 0xFF;        // CRC High byte
    
    
    // Clear any remaining data in buffer before sending
    clearBuffer();
    
    // Enable transmit mode for sending
    enableTransmit();
    
    // Send request
    _serial->write(request, 8);
    _serial->flush();
    delay(10);
    
    // Switch to receive mode
    enableReceive();
    
    // Receive optimized response
    uint8_t response[256];
    uint8_t responseLength = 0;
    uint32_t startTime = millis();
    uint32_t lastByteTime = 0;
    
    // Calculate minimum expected bytes: 3 (header) + 2*numRegs (data) + 2 (CRC)
    uint8_t minBytesExpected = 3 + (2 * numRegs) + 2;

    bool foundSlaveAddr = false;
    
    while (millis() - startTime < _responseTimeout) {
        if (_serial->available()) {
            uint8_t byte = _serial->read();

            if (!foundSlaveAddr && byte == slaveAddr)
                foundSlaveAddr = true;
            
            if (foundSlaveAddr) {
                if (responseLength < sizeof(response)) {
                    response[responseLength] = byte;
                    responseLength++;
                    lastByteTime = millis();
                }
            }
        }
        
        // If received all expected bytes and passed time without new bytes
        if (responseLength >= minBytesExpected && (millis() - lastByteTime) > 10) {
            break;
        }
    }

    if (responseLength == 0) {
        return false;
    }
    
    // Check if it is an error response
    if (response[1] & 0x80) {
        return false;
    }
    
    // Verify CRC
    if (!verifyCRC16(response, responseLength)) {
        return false;
    }
    
    // Extract data
    uint8_t byteCount = response[2];
    uint8_t dataIndex = 0;
    
    for (uint8_t i = 3; i < 3 + byteCount; i += 2) {
        if (dataIndex < numRegs) {
            if (big_endian) {
                // Big endian: high byte first, low byte second
                data[dataIndex] = (response[i] << 8) | response[i + 1];
            } else {
                // Little endian: low byte first, high byte second
                data[dataIndex] = response[i] | (response[i + 1] << 8);
            }
            dataIndex++;
        }
    }
    
    return true;
}

// Method to read input registers (function 0x04)
bool RS485::readInputRegisters(uint8_t slaveAddr, uint16_t startAddr, uint16_t numRegs, uint16_t* data, bool big_endian) {
    
    uint8_t request[8];
    request[0] = slaveAddr;
    request[1] = MODBUS_READ_INPUT_REGISTERS;
    request[2] = (startAddr >> 8) & 0xFF;  // High byte
    request[3] = startAddr & 0xFF;         // Low byte
    request[4] = (numRegs >> 8) & 0xFF;    // High byte
    request[5] = numRegs & 0xFF;           // Low byte
    
    uint16_t crc = calculateCRC16(request, 6);
    request[6] = crc & 0xFF;               // CRC Low byte
    request[7] = (crc >> 8) & 0xFF;        // CRC High byte
    
    
    // Clear any remaining data in buffer before sending
    clearBuffer();
    
    // Enable transmit mode for sending
    enableTransmit();
    
    // Send request
    _serial->write(request, 8);
    _serial->flush();
    delay(10);
    
    // Switch to receive mode
    enableReceive();
    
    // Receive optimized response - exit when all bytes received
    uint8_t response[256];
    uint8_t responseLength = 0;
    uint32_t startTime = millis();
    uint32_t lastByteTime = 0;
    
    // Calculate minimum expected bytes: 3 (header) + 2*numRegs (data) + 2 (CRC)
    uint8_t minBytesExpected = 3 + (2 * numRegs) + 2;

    bool foundSlaveAddr = false;
    
    while (millis() - startTime < _responseTimeout) {
        if (_serial->available()) {
            uint8_t byte = _serial->read();
            
            if (!foundSlaveAddr && byte == slaveAddr)
                foundSlaveAddr = true;
            
            if (foundSlaveAddr) {
                if (responseLength < sizeof(response)) {
                    response[responseLength] = byte;
                    responseLength++;
                    lastByteTime = millis();
                }
            }
        }
        
        // If received all expected bytes and passed time without new bytes
        if (responseLength >= minBytesExpected && (millis() - lastByteTime) > 10) {
            break;
        }
    }

    if (responseLength == 0) {
        return false;
    }
    
    // Check if it is an error response
    if (response[1] & 0x80) {
        return false;
    }
    
    // Verify CRC
    if (!verifyCRC16(response, responseLength)) {
        return false;
    }
    
    // Extract data
    uint8_t byteCount = response[2];
    uint8_t dataIndex = 0;
    
    for (uint8_t i = 3; i < 3 + byteCount; i += 2) {
        if (dataIndex < numRegs) {
            if (big_endian) {
                // Big endian: high byte first, low byte second
                data[dataIndex] = (response[i] << 8) | response[i + 1];
            } else {
                // Little endian: low byte first, high byte second
                data[dataIndex] = response[i] | (response[i + 1] << 8);
            }
            dataIndex++;
        }
    }
    
    return true;
}

// Method to write a single register (function 0x06)
bool RS485::writeSingleRegister(uint8_t slaveAddr, uint16_t regAddr, uint16_t value, bool big_endian) {
    uint8_t request[8];
    request[0] = slaveAddr;
    request[1] = MODBUS_WRITE_SINGLE_REGISTER;
    request[2] = (regAddr >> 8) & 0xFF;    // High byte
    request[3] = regAddr & 0xFF;           // Low byte
    
    if (big_endian) {
        // Big endian: high byte first, low byte second
        request[4] = (value >> 8) & 0xFF;      // High byte
        request[5] = value & 0xFF;             // Low byte
    } else {
        // Little endian: low byte first, high byte second
        request[4] = value & 0xFF;             // Low byte
        request[5] = (value >> 8) & 0xFF;      // High byte
    }
    
    uint16_t crc = calculateCRC16(request, 6);
    request[6] = crc & 0xFF;               // CRC Low byte
    request[7] = (crc >> 8) & 0xFF;        // CRC High byte
    
    
    // Clear any remaining data in buffer before sending
    clearBuffer();
    
    // Enable transmit mode for sending
    enableTransmit();
    
    // Send request
    _serial->write(request, 8);
    _serial->flush();
    delay(10);
    
    // Switch to receive mode
    enableReceive();
    
    // Receive optimized response
    uint8_t response[8];
    uint8_t responseLength = 0;
    uint32_t startTime = millis();
    uint32_t lastByteTime = 0;
    
    // For writeSingleRegister: 1 (address) + 1 (function) + 2 (register addr) + 2 (register value) + 2 (CRC) = 8 bytes minimum
    uint8_t minBytesExpected = 8;

    bool foundSlaveAddr = false;
    
    // 300ms timeout to wait for response, avoid setting _responseTimeout to low
    while (millis() - startTime < 300) {
        if (_serial->available()) {
            uint8_t byte = _serial->read();
            
            if (!foundSlaveAddr && byte == slaveAddr)
                foundSlaveAddr = true;
            
            if (foundSlaveAddr) {
                if (responseLength < sizeof(response)) {
                    response[responseLength] = byte;
                    responseLength++;
                    lastByteTime = millis();
                }
            }
        }
        
        // If received all expected bytes and passed time without new bytes
        if (responseLength >= minBytesExpected && (millis() - lastByteTime) > 10) {
            break;
        }
    }
    
    if (responseLength == 0) {
        return false;
    }
    
    // Check if it is an error response
    if (response[1] & 0x80) {
        return false;
    }
    
    // Verify CRC
    if (!verifyCRC16(response, responseLength)) {
        return false;
    }
    
    return true;
}

// Method to write multiple registers (function 0x10)
bool RS485::writeMultipleRegisters(uint8_t slaveAddr, uint16_t startAddr, uint16_t numRegs, uint16_t* data, bool big_endian) {
    // Calculate total bytes needed: 6 (header) + 1 (byte count) + 2*numRegs (data) + 2 (CRC)
    uint8_t totalBytes = 6 + 1 + (2 * numRegs) + 2;
    uint8_t request[256]; // Buffer for request
    
    if (totalBytes > sizeof(request)) {
        return false; // Request too large
    }
    
    // Build request frame
    request[0] = slaveAddr;
    request[1] = MODBUS_WRITE_MULTIPLE_REGISTERS;
    request[2] = (startAddr >> 8) & 0xFF;  // High byte
    request[3] = startAddr & 0xFF;         // Low byte
    request[4] = (numRegs >> 8) & 0xFF;    // High byte
    request[5] = numRegs & 0xFF;           // Low byte
    request[6] = 2 * numRegs;              // Byte count
    
    // Add data bytes
    uint8_t dataIndex = 7;
    for (uint16_t i = 0; i < numRegs; i++) {
        if (big_endian) {
            // Big endian: high byte first, low byte second
            request[dataIndex++] = (data[i] >> 8) & 0xFF;  // High byte
            request[dataIndex++] = data[i] & 0xFF;         // Low byte
        } else {
            // Little endian: low byte first, high byte second
            request[dataIndex++] = data[i] & 0xFF;         // Low byte
            request[dataIndex++] = (data[i] >> 8) & 0xFF;  // High byte
        }
    }
    
    // Calculate and add CRC
    uint16_t crc = calculateCRC16(request, totalBytes - 2);
    request[totalBytes - 2] = crc & 0xFF;               // CRC Low byte
    request[totalBytes - 1] = (crc >> 8) & 0xFF;        // CRC High byte
    
    // Clear any remaining data in buffer before sending
    clearBuffer();
    
    // Enable transmit mode for sending
    enableTransmit();
    
    // Send request
    _serial->write(request, totalBytes);
    _serial->flush();
    delay(10);
    
    // Switch to receive mode
    enableReceive();
    
    // Receive optimized response
    uint8_t response[8];
    uint8_t responseLength = 0;
    uint32_t startTime = millis();
    uint32_t lastByteTime = 0;
    
    // For writeMultipleRegisters: 1 (address) + 1 (function) + 2 (register addr) + 2 (register quantity) + 2 (CRC) = 8 bytes minimum
    uint8_t minBytesExpected = 8;

    bool foundSlaveAddr = false;
    
    // 300ms timeout to wait for response, avoid setting _responseTimeout to low
    while (millis() - startTime < 300) {
        if (_serial->available()) {
            uint8_t byte = _serial->read();
            
            if (!foundSlaveAddr && byte == slaveAddr)
                foundSlaveAddr = true;
            
            if (foundSlaveAddr) {
                if (responseLength < sizeof(response)) {
                    response[responseLength] = byte;
                    responseLength++;
                    lastByteTime = millis();
                }
            }
        }
        
        // If received all exected bytes and passed time without new bytes
        if (responseLength >= minBytesExpected && (millis() - lastByteTime) > 10) {
            break;
        }
    }

    if (responseLength == 0) {
        return false;
    }
    
    // Check if it is an error response
    if (response[1] & 0x80) {
        return false;
    }
    
    // Verify CRC
    if (!verifyCRC16(response, responseLength)) {
        return false;
    }
    
    return true;
}

// Method to reset energy (function 0x42)
bool RS485::resetEnergy(uint8_t slaveAddr) {
    uint8_t request[4];
    request[0] = slaveAddr;
    request[1] = MODBUS_RESET_ENERGY;
    
    uint16_t crc = calculateCRC16(request, 2);
    request[2] = crc & 0xFF;               // CRC Low byte
    request[3] = (crc >> 8) & 0xFF;        // CRC High byte
    
    
    // Clear any remaining data in buffer before sending
    clearBuffer();
    
    // Enable transmit mode for sending
    enableTransmit();
    
    // Send request
    _serial->write(request, 4);
    _serial->flush();
    delay(10);
    
    // Switch to receive mode
    enableReceive();
    
    // Receive optimized response
    uint8_t response[4];
    uint8_t responseLength = 0;
    uint32_t startTime = millis();
    uint32_t lastByteTime = 0;
    
    // For resetEnergy: 1 (address) + 1 (function) + 2 (CRC) = 4 bytes minimum
    uint8_t minBytesExpected = 4;

    bool foundSlaveAddr = false;
    
    // 300ms timeout to wait for response, avoid setting _responseTimeout to low
    while (millis() - startTime < 300) {
        if (_serial->available()) {
            uint8_t byte = _serial->read();
            
            if (!foundSlaveAddr && byte == slaveAddr)
                foundSlaveAddr = true;
            
            if (foundSlaveAddr) {
                if (responseLength < sizeof(response)) {
                    response[responseLength] = byte;
                    responseLength++;
                    lastByteTime = millis();
                }
            }
        }
        
        // If received all expected bytes and passed time without new bytes
        if (responseLength >= minBytesExpected && (millis() - lastByteTime) > 10) {
            break;
        }
    }
    
    
    if (responseLength == 0) {
        return false;
    }
    
    // Check if it is an error response
    if (response[1] & 0x80) {
        return false;
    }
    
    // Verify CRC
    if (!verifyCRC16(response, responseLength)) {
        return false;
    }
    
    return true;
}

// Method to reset energy with phase sequence (for PZEM-6L24)
bool RS485::resetEnergy(uint8_t slaveAddr, uint8_t phaseSequence) {
    uint8_t request[6];
    request[0] = slaveAddr;
    request[1] = MODBUS_RESET_ENERGY;
    request[2] = 0x00;  // Reserved byte
    request[3] = phaseSequence;  // Phase sequence byte
    
    uint16_t crc = calculateCRC16(request, 4);
    request[4] = crc & 0xFF;               // CRC Low byte
    request[5] = (crc >> 8) & 0xFF;        // CRC High byte
    
    
    // Clear any remaining data in buffer before sending
    clearBuffer();
    
    // Enable transmit mode for sending
    enableTransmit();
    
    // Send request
    _serial->write(request, 6);
    _serial->flush();
    delay(10);
    
    // Switch to receive mode
    enableReceive();
    
    // Receive optimized response
    uint8_t response[6];
    uint8_t responseLength = 0;
    uint32_t startTime = millis();
    uint32_t lastByteTime = 0;
    
    // For resetEnergy with phase: 1 (address) + 1 (function) + 1 (reserved) + 1 (phase) + 2 (CRC) = 6 bytes minimum
    uint8_t minBytesExpected = 6;

    bool foundSlaveAddr = false;
    
    // 300ms timeout to wait for response, avoid setting _responseTimeout to low
    while (millis() - startTime < 300) {
        if (_serial->available()) {
            uint8_t byte = _serial->read();
            
            if (!foundSlaveAddr && byte == slaveAddr)
                foundSlaveAddr = true;
            
            if (foundSlaveAddr) {
                if (responseLength < sizeof(response)) {
                    response[responseLength] = byte;
                    responseLength++;
                    lastByteTime = millis();
                }
            }
        }
        
        // If received all expected bytes and passed time without new bytes
        if (responseLength >= minBytesExpected && (millis() - lastByteTime) > 10) {
            break;
        }
    }
    
    
    if (responseLength == 0) {
        return false;
    }
    
    // Check if it is an error response (0xC2 indicates error)
    if (response[1] == 0xC2) {
        return false;
    }
    
    // Verify CRC
    if (!verifyCRC16(response, responseLength)) {
        return false;
    }
    
    return true;
}

// Modbus CRC16 calculation
uint16_t RS485::calculateCRC16(uint8_t* data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    
    return crc;
}

// CRC16 verification
bool RS485::verifyCRC16(uint8_t* data, uint8_t length) {
    if (length < 2) {
        return false;
    }
    
    uint16_t calculatedCRC = calculateCRC16(data, length - 2);
    uint16_t receivedCRC = (data[length - 1] << 8) | data[length - 2];
    
    return (calculatedCRC == receivedCRC);
}

// Configure timeouts
void RS485::setTimeouts(uint32_t responseTimeout) {
    _responseTimeout = responseTimeout;
}

// Set enable pin for MAX485
bool RS485::setEnable(uint8_t enablePin) {
    if (_rs485_en >= 0) {
        _rs485_en = enablePin;
        pinMode(_rs485_en, OUTPUT);
        enableReceive(); // Start in receive mode
        return true;
    }
    return false;
}

// Enable transmit mode (DE/RE = HIGH)
void RS485::enableTransmit() {
    if (_rs485_en >= 0) {
        digitalWrite(_rs485_en, HIGH);
        delay(1); // Small delay to ensure mode change
    }
}

// Enable receive mode (DE/RE = LOW)
void RS485::enableReceive() {
    if (_rs485_en >= 0) {
        digitalWrite(_rs485_en, LOW);
        delay(1); // Small delay to ensure mode change
    }
}

// Clear any remaining data in buffer
void RS485::clearBuffer() {
    while (_serial->available()) {
        _serial->read();
    }
}

// Combine two 16-bit registers into a 32-bit value (unsigned or signed)
uint32_t RS485::combineRegisters(uint16_t low, uint16_t high, bool signed_result) {
    uint32_t combined = ((uint32_t)high << 16) | low;
    
    if (signed_result) {
        // Convert to signed 32-bit integer
        return (int32_t)combined;
    }
    
    return combined;
}

// Get serial reference
Stream* RS485::getSerial() {
    return _serial;
}
