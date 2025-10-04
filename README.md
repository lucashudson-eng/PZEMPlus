# PZEMPlus

![Version](https://img.shields.io/badge/version-0.4.0-blue.svg)
![License](https://img.shields.io/badge/license-GPL--3.0-green.svg)
![Platform](https://img.shields.io/badge/platform-Arduino%20%7C%20ESP32-orange.svg)

PZEMPlus is an Arduino/ESP32 library to easily read data from Peacefair energy monitoring devices.

## Supported Devices
- PZEM-004T
- PZEM-6L24  
- PZIOT-E02
- PZEM-003
- PZEM-017

## Installation
Install via Arduino Library Manager or download from GitHub releases.

## Features

### Version 0.4.0
- **PZEM-004T Fully Implemented**: Complete implementation with all measurement functions
- **RS485 Communication Layer**: Robust Modbus RTU communication with CRC verification
- **Multi-Device Architecture**: Framework for multiple PZEM models (PZEM-004T ready, others in development)
- **Comprehensive Measurements**: Voltage, current, power, energy, frequency, power factor
- **Device Configuration**: Alarm thresholds, address setting, energy counter reset
- **Efficient Reading**: Batch reading method for all measurements in a single call
- **Debug Support**: Built-in debugging capabilities for troubleshooting communication issues

## Usage

### Basic Setup
```cpp
#define PZEM_004T  // Define your PZEM model

#include <PZEMPlus.h>

PZEMPlus pzem(Serial2);
```

### Reading Measurements
```cpp
// Read individual measurements
float voltage = pzem.readVoltage();
float current = pzem.readCurrent();
float power = pzem.readPower();
float energy = pzem.readEnergy();
float frequency = pzem.readFrequency();
float powerFactor = pzem.readPowerFactor();
bool alarm = pzem.readAlarmStatus();

// Read all measurements at once (more efficient)
float voltage, current, power, energy, frequency, powerFactor;
bool alarm;
if (pzem.readAll(&voltage, &current, &power, &energy, &frequency, &powerFactor, &alarm)) {
    // All measurements read successfully
    Serial.println("Voltage: " + String(voltage) + "V");
    Serial.println("Current: " + String(current) + "A");
    Serial.println("Power: " + String(power) + "W");
    Serial.println("Energy: " + String(energy) + "kWh");
}
```

### Device Configuration
```cpp
// Set alarm threshold
pzem.setPowerAlarm(1000); // 1000W threshold

// Change device address
pzem.setAddress(0x01);

// Reset energy counter
pzem.resetEnergy();

// Get current settings
uint16_t threshold = pzem.getPowerAlarm();
uint8_t address = pzem.getAddress();
```

### Debugging and Troubleshooting
```cpp
// Enable debug mode for communication troubleshooting
pzem.setDebug(true);

// Configure communication timeouts (default: 100ms)
pzem.setTimeouts(2000); // 2 seconds timeout

// Configure sample time for optimized readings
pzem.setSampleTime(1000); // Read from device every 1000ms, use cache otherwise
// pzem.setSampleTime(0);  // Disable caching, read directly from device

// Debug output will show:
// - Request/response data in hexadecimal
// - Communication errors and timeouts
// - CRC verification results
// - Sample time cache hits/misses
```

## Supported Models

### Fully Implemented
- **PZEM-004T**: Single-phase energy monitor with complete functionality
  - All measurement parameters (voltage, current, power, energy, frequency, power factor)
  - Device configuration (alarm threshold, address setting)
  - Energy counter reset functionality
  - Batch reading for efficiency

### In Development
- **PZEM-6L24**: 3-phase energy monitor (class structure ready)
- **PZIOT-E02**: IoT energy monitor (class structure ready)
- **PZEM-003**: 3-phase energy monitor (class structure ready)
- **PZEM-017**: 3-phase energy monitor (class structure ready)

### Implementation Status
- ✅ **PZEM-004T**: Complete implementation with full feature set
- 🚧 **PZEM-6L24**: Class structure created, implementation pending
- 🚧 **PZIOT-E02**: Class structure created, implementation pending
- 🚧 **PZEM-003**: Class structure created, implementation pending
- 🚧 **PZEM-017**: Class structure created, implementation pending