# PZEMPlus

![Version](https://img.shields.io/badge/version-0.7.0-blue.svg)
![License](https://img.shields.io/badge/license-GPL--3.0-green.svg)
![Platform](https://img.shields.io/badge/platform-Arduino%20%7C%20ESP32-orange.svg)

PZEMPlus is an Arduino/ESP32 library to easily read data from Peacefair energy monitoring devices.

## Supported Devices

### AC Energy Monitors
- PZEM-004T (Single-phase AC, 10A built-in shunt, 100A external shunt)
- PZEM-014 (Single-phase AC, 10A built-in shunt, same functionality as PZEM-004T)
- PZEM-016 (Single-phase AC, 100A external shunt, same functionality as PZEM-004T)
- PZEM-6L24 (3-phase AC, 100A external shunt)
- PZIOT-E02 (IoT single-phase AC monitor, 100A built-in shunt)

### DC Power Monitors
- PZEM-003 (DC, 10A range, built-in shunt)
- PZEM-017 (DC, 50A/100A/200A/300A range, external shunt, extends from PZEM-003 to add current range)

## Installation
Install via Arduino or PlatformIO Library Manager, or download from GitHub releases.

## Usage

### Basic Setup (Single Device)

#### For AC Energy Monitors (PZEM-004T/PZEM-014/PZEM-016/PZEM-6L24)
```cpp
#define PZEM_004T
// #define PZEM_014
// #define PZEM_016
// #define PZEM_6L24

#include <PZEMPlus.h>

PZEMPlus pzem(Serial2);
```

#### For DC Energy Monitors (PZEM-003/017)
```cpp
#define PZEM_003
// #define PZEM_017

#include <PZEMPlus.h>

PZEMPlus pzem(Serial2);

// For MAX485 module (optional)
// pzem.setEnable(4); // Set enable pin for RS485 transceiver
```

### Multi Device Setup

#### Managing Multiple PZEM Devices
```cpp
#define PZEM_004T  // Exampling with PZEM-004T

#include <PZEMPlus.h>

// Number of PZEM devices to use
#define NUM_DEVICES 3

// Create array of PZEMPlus objects
PZEMPlus* pzemDevices[NUM_DEVICES];

void setup() {
  // Initialize each device with different addresses
  for (int i = 0; i < NUM_DEVICES; i++) {
    uint8_t address = i+1;
    pzemDevices[i] = new PZEMPlus(Serial2, address);
    pzemDevices[i]->begin();
  }
}

void loop() {
  // Read all measurements from all devices
  for (int i = 0; i < NUM_DEVICES; i++) {
    float voltage, current, power, energy, frequency, powerFactor;
    
    if (pzemDevices[i]->readAll(&voltage, &current, &power, &energy, &frequency, &powerFactor)) {
      Serial.print("Device ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(voltage, 1);
      Serial.println("V");
    }
  }
  delay(2000);
}
```


### Reading Measurements

#### For AC Energy Monitors (PZEM-004T/PZEM-014/PZEM-016)
```cpp
// Read individual measurements
float voltage = pzem.readVoltage();
float current = pzem.readCurrent();
float power = pzem.readPower();
float energy = pzem.readEnergy();
float frequency = pzem.readFrequency();
float powerFactor = pzem.readPowerFactor();
bool alarm = pzem.readPowerAlarm();

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

#### For 3-Phase AC Energy Monitors (PZEM-6L24)
```cpp
// Read individual phase measurements (0=A, 1=B, 2=C)
float voltageA = pzem.readVoltage(0);
float currentA = pzem.readCurrent(0);
float powerA = pzem.readActivePower(0);
float energyA = pzem.readActiveEnergy(0);

// Read all phases at once (more efficient)
float voltageA, voltageB, voltageC;
pzem.readVoltage(voltageA, voltageB, voltageC);

float currentA, currentB, currentC;
pzem.readCurrent(currentA, currentB, currentC);

// Read combined measurements (total across all phases)
float totalPower = pzem.readActivePower();
float totalEnergy = pzem.readActiveEnergy();

// Read phase angles
float voltageAngleA, voltageAngleB, voltageAngleC;
pzem.readVoltagePhaseAngle(voltageAngleA, voltageAngleB, voltageAngleC);

// Reset energy (by phase, combined or all)
pzem.resetEnergy(PZEM_RESET_ENERGY_ALL);
```

### Device Configuration

#### For AC Energy Monitors (PZEM-004T/PZEM-014/PZEM-016)
```cpp
// Set power alarm threshold (1W precision for alarm, 0.1W for measurements)
pzem.setPowerAlarm(2300.0); // 2300W threshold

// Change device address
pzem.setAddress(0x01);

// Reset energy counter
pzem.resetEnergy();

// Get current settings
float threshold = pzem.getPowerAlarm(); // Returns 2300.0W
uint8_t address = pzem.getAddress();
```

#### For 3-Phase AC Energy Monitors (PZEM-6L24)
```cpp
// Change device software address
pzem.setAddress(0x01);

// Use of hardware address
pzem.setAddress(0x00);

// Set baudrate and connection type (same register)
pzem.setBaudrateAndConnectionType(9600, PZEM_CONNECTION_3PHASE_4WIRE);

// Set frequency system
pzem.setFrequency(50); // or 60 for 60Hz

// Reset energy counter (by phase, combined or all)
pzem.resetEnergy(PZEM_RESET_ENERGY_ALL); // Reset all phases

// Get current settings
uint8_t address = pzem.getAddress(); // Returns 0x01 (or 0xFF on error)
bool software = pzem.getSoftwareHardwareSettings(); // Returns true (software) or false (hardware)
uint32_t baudrate = pzem.getBaudrate(); // Returns 9600 (or 0 on error)
uint8_t connection = pzem.getConnectionType(); // Returns 0 (3 phases 4 wires) or 1 (3 phases 3 wires), or 0xFF on error
uint8_t frequency = pzem.getFrequency(); // Returns 50 or 60 (Hz), or 0 on error
```

#### For DC Energy Monitors (PZEM-003/017)
```cpp
// For MAX485 module (optional)
// pzem.setEnable(4); // Set enable pin for RS485 transceiver

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
float highThreshold = pzem.getHighVoltageAlarm(); // Returns 300.00V
float lowThreshold = pzem.getLowVoltageAlarm();   // Returns 7.00V
uint8_t address = pzem.getAddress(); // Returns 0x01
uint16_t currentRange = pzem.getCurrentRange(); // Returns 300A (PZEM-017 only)
```

## Precision and Resolutions

### PZEM-004T/PZEM-014/PZEM-016 (AC Energy Monitors)

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

### PZEM-6L24 (3-Phase AC Energy Monitor)

| Parameter | Resolution | Accuracy | Min Value | Max Value | Unit |
|-----------|------------|----------|-----------|-----------|------|
| Voltage | 0.1V | ±1% | 50V | 566V | V |
| Current | 0.01A | ±1% | 0A | 100A | A |
| Frequency | 0.01Hz | ±1% | 45Hz | 65Hz | Hz |
| Active Power | 0.1W | ±1% | 0W | 38kW | kW |
| Reactive Power | 0.1Var | ±1% | 0Var | 38kVar | kVar |
| Apparent Power | 0.1VA | ±1% | 0VA | 38kVA | kVA |
| Power Factor | 0.01 | ±1% | 0.00 | 1.00 | - |
| Active Energy | 0.1kWh | ±1% | 0kWh | 399999999.9kWh | kWh |
| Reactive Energy | 0.1kVarh | ±1% | 0kVarh | 399999999.9kVarh | kVarh |
| Apparent Energy | 0.1kVAh | ±1% | 0kVAh | 399999999.9kVAh | kVAh |
| Phase Angle | 0.01° | - | 0° | 360° | ° |

### Troubleshooting
```cpp
// Configure communication timeouts (default: 100ms)
pzem.setTimeouts(100); // 100ms timeout

// For PZEM-003/017 with MAX485 module
// pzem.setEnable(4); // Set enable pin for RS485 transceiver
```

## Examples

The library includes comprehensive examples for all supported devices:

- **PZEM-004T**: `examples/pzem_004t/pzem_004t.ino` - Single-phase energy monitoring (also works for PZEM-014 and PZEM-016)
- **Multi-Device**: `examples/multiDevice/multiDevice.ino` - Multiple devices management example with PZEM-004T
- **Address Change**: `examples/changeAddress/changeAddress.ino` - Device address configuration
- **PZEM-003**: `examples/pzem_003/pzem_003.ino` - DC energy monitoring (PZEM-003)
- **PZEM-017**: `examples/pzem_017/pzem_017.ino` - DC energy monitoring (PZEM-017 with current range)
- **PZEM-6L24**: `examples/pzem_6l24/pzem_6l24.ino` - Three-phase energy monitoring

## Supported Models

### Fully Implemented
- **PZEM-004T**: Single-phase AC energy monitor with complete functionality (10A range, built-in shunt/100A range, external shunt)
  - All measurement parameters (voltage, current, power, energy, frequency, power factor)
  - Device configuration (power alarm threshold, address setting)
  - Energy counter reset functionality
  - Batch reading for efficiency

- **PZEM-014**: Single-phase AC energy monitor with complete functionality (10A range, built-in shunt)
  - Same functionality as PZEM-004T
  - All measurement parameters (voltage, current, power, energy, frequency, power factor)
  - Device configuration (power alarm threshold, address setting)
  - Energy counter reset functionality
  - Batch reading for efficiency

- **PZEM-016**: Single-phase AC energy monitor with complete functionality (100A range, external shunt)
  - Same functionality as PZEM-004T
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

- **PZEM-6L24**: 3-phase AC energy monitor (100A range, external shunt)
  - Read functions implemented (voltage, current, power, energy, frequency, power factor, phase angles)
  - Individual phase measurements (A, B, C)
  - Multi-phase batch reading methods
  - Combined measurements (total across all phases)
  - Energy reset functionality
  - Device configuration functions

### In Development
- **PZIOT-E02**: IoT Single-phase AC energy monitor (100A range, built-in shunt)

### Implementation Status
- ✅ **PZEM-004T**: Complete implementation with full feature set
- ✅ **PZEM-014**: Complete implementation with full feature set (inherits from PZEM-004T)
- ✅ **PZEM-016**: Complete implementation with full feature set (inherits from PZEM-004T)
- ✅ **PZEM-003**: Complete implementation with full feature set  
- ✅ **PZEM-017**: Complete implementation with full feature set (extends from PZEM-003 to add current range)
- ✅ **PZEM-6L24**: Complete implementation with full feature set
- 🚧 **PZIOT-E02**: Class structure created, implementation pending

