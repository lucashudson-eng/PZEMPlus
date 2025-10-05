# PZEMPlus

![Version](https://img.shields.io/badge/version-0.5.3-blue.svg)
![License](https://img.shields.io/badge/license-GPL--3.0-green.svg)
![Platform](https://img.shields.io/badge/platform-Arduino%20%7C%20ESP32-orange.svg)

PZEMPlus is an Arduino/ESP32 library to easily read data from Peacefair energy monitoring devices.

## Supported Devices

### AC Energy Monitors
- PZEM-004T (Single-phase AC, 10A built-in shunt, 100A external shunt)
- PZEM-6L24 (3-phase AC, 100A external shunt)
- PZIOT-E02 (IoT single-phase AC monitor, 100A built-in shunt)

### DC Power Monitors
- PZEM-003 (DC, 10A range, built-in shunt)
- PZEM-017 (DC, 50A/100A/200A/300A range, external shunt)

## Installation
Install via Arduino Library Manager or download from GitHub releases. 

## Usage

### Basic Setup

#### For AC Energy Monitors (PZEM-004T)
```cpp
#define PZEM_004T  // Define your PZEM model

#include <PZEMPlus.h>

PZEMPlus pzem(Serial2);
```

#### For DC Energy Monitors (PZEM-003/017)
```cpp
#define PZEM_003
// #define PZEM_017

#include <PZEMPlus.h>

PZEM003017 pzem(Serial2);
```

### Reading Measurements

#### For AC Energy Monitors (PZEM-004T)
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

#### For DC Energy Monitors (PZEM-003/017)
```cpp
// Read individual measurements
float voltage = pzem.readVoltage();
float current = pzem.readCurrent();
float power = pzem.readPower();
float energy = pzem.readEnergy();
bool highVoltageAlarm = pzem.readHighVoltageAlarm();
bool lowVoltageAlarm = pzem.readLowVoltageAlarm();

// Read all measurements at once (more efficient)
float voltage, current, power, energy;
if (pzem.readAll(&voltage, &current, &power, &energy)) {
    // All measurements read successfully
    Serial.println("Voltage: " + String(voltage) + "V");
    Serial.println("Current: " + String(current) + "A");
    Serial.println("Power: " + String(power) + "W");
    Serial.println("Energy: " + String(energy) + "Wh");
}
```

### Device Configuration

#### For AC Energy Monitors (PZEM-004T)
```cpp
// Set power alarm threshold (1W precision for alarm, 0.1W for measurements)
pzem.setPowerAlarm(2300.0); // 2300W threshold

// Change device address
pzem.setAddress(0x01);

// Reset energy counter
pzem.resetEnergy();

// Get current settings
float threshold = pzem.getPowerAlarm(); // Returns 1000.0W
uint8_t address = pzem.getAddress();
```

#### For DC Energy Monitors (PZEM-003/017)
```cpp
// Set voltage alarm thresholds (0.01V precision)
pzem.setHighVoltageAlarm(300.0); // 300.00V threshold
pzem.setLowVoltageAlarm(7.0);  // 7.00V threshold

// Change device address
pzem.setAddress(0x01);

// Set current range (PZEM-017 only)
pzem.setCurrentRange(300); // 300A range

// Reset energy counter
pzem.resetEnergy();

// Get current settings
float highThreshold = pzem.getHighVoltageAlarm(); // Returns 15.00V
float lowThreshold = pzem.getLowVoltageAlarm();   // Returns 10.00V
uint8_t address = pzem.getAddress();
uint16_t currentRange = pzem.getCurrentRange(); // PZEM-017 only
```

## Precision and Resolutions

### PZEM-004T (AC Energy Monitor)

| Parameter | Resolution | Accuracy | Min Value | Max Value | Unit |
|-----------|------------|----------|-----------|-----------|------|
| Voltage | 0.1V | ±0.5% | 80V | 260V | V |
| Current | 0.001A | ±0.5% | 0.01A (built-in) / 0.02A (external) | 10A (built-in) / 100A (external) | A |
| Power | 0.1W | ±0.5% | 0.4W | 2300W (built-in) / 23000W (external) | W |
| Energy | 1Wh | ±0.5% | 0Wh | 9999999Wh | Wh |
| Frequency | 0.1Hz | ±0.5% | 45Hz | 65Hz | Hz |
| Power Factor | 0.01 | ±1% | 0.00 | 1.00 | - |

### PZEM-003 (DC Energy Monitor)

| Parameter | Resolution | Accuracy | Min Value | Max Value | Unit |
|-----------|------------|----------|-----------|-----------|------|
| Voltage | 0.01V | ±1% | 0.05V | 300V | V |
| Current | 0.01A | ±1% | 0.01A | 10A | A |
| Power | 0.1W | ±1% | 0.1W | 3000W | W |
| Energy | 1Wh | ±1% | 0Wh | 9999999Wh | Wh |

### PZEM-017 (DC Energy Monitor)

| Parameter | Resolution | Accuracy | Min Value | Max Value | Unit |
|-----------|------------|----------|-----------|-----------|------|
| Voltage | 0.01V | ±1% | 0.05V | 300V | V |
| Current | 0.01A | ±1% | 0.02A | 300A | A |
| Power | 0.1W | ±1% | 0.2W | 90000W | W |
| Energy | 1Wh | ±1% | 0Wh | 9999999Wh | Wh |

**Current Range Options for PZEM-017:**
- 50A range
- 100A range
- 200A range
- 300A range

### Troubleshooting
```cpp
// Configure communication timeouts (default: 100ms)
pzem.setTimeouts(100); // 100ms timeout

// Configure sample time for optimized readings
// pzem.setSampleTime(1000); // Read from device every 1000ms, use cache otherwise
pzem.setSampleTime(0);  // Disable caching, read directly from device
```

## Supported Models

### Fully Implemented
- **PZEM-004T**: Single-phase AC energy monitor with complete functionality (10A range, built-in shunt/100A range, external shunt)
  - All measurement parameters (voltage, current, power, energy, frequency, power factor)
  - Device configuration (power alarm threshold, address setting)
  - Energy counter reset functionality
  - Batch reading for efficiency

- **PZEM-003**: DC energy monitor with complete functionality (10A range, built-in shunt)
  - DC measurement parameters (voltage, current, power, energy)
  - Voltage alarm system (high and low voltage thresholds)
  - Device configuration (voltage alarm thresholds, address setting)
  - Energy counter reset functionality
  - Batch reading for efficiency

- **PZEM-017**: DC energy monitor with complete functionality (50A-300A range, external shunt)
  - DC measurement parameters (voltage, current, power, energy)
  - Voltage alarm system (high and low voltage thresholds)
  - Current range configuration (50A, 100A, 200A, 300A)
  - Device configuration (voltage alarm thresholds, current range, address setting)
  - Energy counter reset functionality
  - Batch reading for efficiency

### In Development
- **PZEM-6L24**: 3-phase AC energy monitor (100A range, external shunt)
- **PZIOT-E02**: IoT Single-phase AC energy monitor (100A range, built-in shunt)

### Implementation Status
- ✅ **PZEM-004T**: Complete implementation with full feature set
- ✅ **PZEM-003**: Complete implementation with full feature set
- ✅ **PZEM-017**: Complete implementation with full feature set
- 🚧 **PZEM-6L24**: Class structure created, implementation pending
- 🚧 **PZIOT-E02**: Class structure created, implementation pending