#include "PZEM6L24.h"
#include <math.h>  // for NAN and isnan()

/*
 * PZEM6L24.cpp
 * Driver for the Peacefair PZEM-6L24 three-phase energy meter.
 *
 * This implementation wraps a Modbus/RS-485 transport (via RS485 base class)
 * and exposes typed read helpers for voltage, current, frequency, power,
 * energy and phase angles — both per-phase (A/B/C) and combined.
 *
 * Error handling: all single-value readers return IEEE-754 NAN on failure
 * (e.g., timeout). Batch readers set their out-parameters to NAN on failure.
 *
 * Scaling: raw register values are converted to engineering units using
 * constants like PZEM_VOLTAGE_RESOLUTION, PZEM_POWER_RESOLUTION, etc.
 * Each function lists the device register addresses and LSB resolution.
 */

// =============================================================================
// Constructors
// =============================================================================

#if defined(__AVR_ATmega328P__)
/**
 * @brief Construct using a SoftwareSerial transport (AVR UNO/Nano class MCUs).
 * @param serial  Reference to an already-constructed SoftwareSerial object.
 * @param slaveAddr Modbus slave address of the PZEM-6L24 device.
 */
PZEM6L24::PZEM6L24(SoftwareSerial &serial, uint8_t slaveAddr)
    : RS485(&serial), _slaveAddr(slaveAddr) {}
#else
/**
 * @brief Construct using a HardwareSerial transport (no explicit pin mapping).
 * @param serial  Reference to a HardwareSerial (e.g., Serial1/Serial2).
 * @param slaveAddr Modbus slave address of the PZEM-6L24 device.
 * @note Use the 4-arg overload if you need to assign RX/TX pins (ESP32, etc.).
 */
PZEM6L24::PZEM6L24(HardwareSerial &serial, uint8_t slaveAddr)
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(-1), _txPin(-1) {}

/**
 * @brief Construct using a HardwareSerial transport with explicit RX/TX pins.
 * @param serial  Reference to a HardwareSerial (e.g., Serial1).
 * @param rxPin   GPIO used for UART RX to the PZEM-6L24 (platform-specific).
 * @param txPin   GPIO used for UART TX to the PZEM-6L24 (platform-specific).
 * @param slaveAddr Modbus slave address of the PZEM-6L24 device.
 */
PZEM6L24::PZEM6L24(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr)
    : RS485(&serial), _slaveAddr(slaveAddr), _rxPin(rxPin), _txPin(txPin) {}
#endif

/**
 * @brief Initialize the serial transport and clear internal buffers.
 * @param baudrate UART baud rate for the PZEM-6L24 (default 9600).
 */
void PZEM6L24::begin(uint32_t baudrate) {
#if defined(__AVR_ATmega328P__)
    ((SoftwareSerial*)getSerial())->begin(baudrate);
#else
    if (_rxPin != -1 && _txPin != -1) {
        ((HardwareSerial*)getSerial())->begin(baudrate, SERIAL_8N1, _rxPin, _txPin);
    } else {
        ((HardwareSerial*)getSerial())->begin(baudrate);
    }
#endif
    clearBuffer();
}

// =============================================================================
// Single-phase measurement readers (phase: 0=A, 1=B, 2=C)
// =============================================================================

/**
 * @brief Read line voltage for a single phase.
 * @param phase 0 for Phase A, 1 for Phase B, 2 for Phase C.
 * @return Voltage in volts (V), or NAN on failure.
 * @note Registers: A=0x0000, B=0x0001, C=0x0002. Resolution: 0.1 V/LSB (unsigned).
 */
float PZEM6L24::readVoltage(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG + phase, 1, data, RS485::RegByteOrder::HighByteSecond)) {
        return data[0] * PZEM_VOLTAGE_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read line current for a single phase.
 * @param phase 0 for Phase A, 1 for Phase B, 2 for Phase C.
 * @return Current in amperes (A), or NAN on failure.
 * @note Registers: A=0x0003, B=0x0004, C=0x0005. Resolution: 0.01 A/LSB (unsigned).
 */
float PZEM6L24::readCurrent(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_REG + phase, 1, data, RS485::RegByteOrder::HighByteSecond)) {
        return data[0] * PZEM_CURRENT_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read line frequency for a single phase.
 * @param phase 0 for Phase A, 1 for Phase B, 2 for Phase C.
 * @return Frequency in hertz (Hz), or NAN on failure.
 * @note Registers: A=0x0006, B=0x0007, C=0x0008. Resolution: 0.01 Hz/LSB.
 */
float PZEM6L24::readFrequency(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_FREQUENCY_REG + phase, 1, data, RS485::RegByteOrder::HighByteSecond)) {
        return data[0] * PZEM_FREQUENCY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read voltage phase angle for a single phase.
 * Phase A is the reference (0°). For B/C, values are read from the meter.
 * @param phase 0=A, 1=B, 2=C.
 * @return Angle in degrees (°), or NAN on failure.
 * @note Registers: B=0x0009, C=0x000A relative to A. Resolution: 0.01°/LSB.
 */
float PZEM6L24::readVoltagePhaseAngle(uint8_t phase) {
    if (phase > 2) return NAN;
    if (phase == 0) return 0.0f; // Phase A is the reference
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_PHASE_REG + (phase - 1), 1, data, RS485::RegByteOrder::HighByteSecond)) {
        return data[0] * PZEM_PHASE_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read current phase angle for a single phase.
 * @param phase 0=A, 1=B, 2=C.
 * @return Angle in degrees (°), or NAN on failure.
 * @note Registers: A=0x000B, B=0x000C, C=0x000D. Resolution: 0.01°/LSB (relative to A voltage).
 */
float PZEM6L24::readCurrentPhaseAngle(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_PHASE_REG + phase, 1, data, RS485::RegByteOrder::HighByteSecond)) {
        return data[0] * PZEM_PHASE_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read active power (per phase).
 * @param phase 0=A, 1=B, 2=C.
 * @return Active power in watts (W), or NAN on failure.
 * @note Registers (low, high): A=(0x000E,0x000F), B=(0x0010,0x0011), C=(0x0012,0x0013). Resolution: 0.1 W/LSB (signed 32-bit from two 16-bit regs).
 */
float PZEM6L24::readActivePower(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_POWER_REG + (phase * 2), 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t uPowerRaw = combineRegisters(data[1], data[0]);
        int32_t  sPowerRaw = (int32_t)uPowerRaw; // signed per datasheet
        return sPowerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read reactive power (per phase).
 * @param phase 0=A, 1=B, 2=C.
 * @return Reactive power in var (may be negative), or NAN on failure.
 * @note Registers (low, high): A=(0x0014,0x0015), B=(0x0016,0x0017), C=(0x0018,0x0019). Resolution: 0.1 var/LSB (signed).
 */
float PZEM6L24::readReactivePower(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_POWER_REG + (phase * 2), 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t uPowerRaw = combineRegisters(data[1], data[0]);
        int32_t  sPowerRaw = (int32_t)uPowerRaw;
        return sPowerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read apparent power (per phase).
 * @param phase 0=A, 1=B, 2=C.
 * @return Apparent power in volt-amps (VA), or NAN on failure.
 * @note Registers (low, high): A=(0x001A,0x001B), B=(0x001C,0x001D), C=(0x001E,0x001F). Resolution: 0.1 VA/LSB (signed).
 */
float PZEM6L24::readApparentPower(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_POWER_REG + (phase * 2), 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t uPowerRaw = combineRegisters(data[1], data[0]);
        int32_t  sPowerRaw = (int32_t)uPowerRaw;
        return sPowerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

// =============================================================================
// Combined measurements (all phases)
// =============================================================================

/**
 * @brief Read system active power (combined A+B+C).
 * @return Active power in watts (W), or NAN on failure.
 * @note Registers (low, high): (0x0020, 0x0021). Resolution: 0.1 W/LSB (signed).
 */
float PZEM6L24::readActivePower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_POWER_COMBINED_REG, 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t uPowerRaw = combineRegisters(data[1], data[0]);
        int32_t  sPowerRaw = (int32_t)uPowerRaw;
        return sPowerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read system reactive power (combined A+B+C).
 * @return Reactive power in var, or NAN on failure.
 * @note Registers (low, high): (0x0022, 0x0023). Resolution: 0.1 var/LSB (signed).
 */
float PZEM6L24::readReactivePower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_POWER_COMBINED_REG, 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t uPowerRaw = combineRegisters(data[1], data[0]);
        int32_t  sPowerRaw = (int32_t)uPowerRaw;
        return sPowerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read system apparent power (combined A+B+C).
 * @return Apparent power in volt-amps (VA), or NAN on failure.
 * @note Registers (low, high): (0x0024, 0x0025). Resolution: 0.1 VA/LSB (signed).
 */
float PZEM6L24::readApparentPower() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_POWER_COMBINED_REG, 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t uPowerRaw = combineRegisters(data[1], data[0]);
        int32_t  sPowerRaw = (int32_t)uPowerRaw;
        return sPowerRaw * PZEM_POWER_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read power factor for a single phase.
 * @param phase 0=A, 1=B, 2=C.
 * @return Power factor (0.00–1.00), or NAN on failure.
 * @note Registers: A/B=0x0026 (hi=A, lo=B); C=high byte of 0x0027. Resolution: 0.01/LSB.
 */
float PZEM6L24::readPowerFactor(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[1];
    if (phase == 0 || phase == 1) {
        if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_A_B_REG, 1, data, RS485::RegByteOrder::HighByteSecond)) {
            if (phase == 0) {
                uint8_t factorRaw = (data[0] >> 8) & 0xFF; // A = high byte of 0x0026
                return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
            } else {
                uint8_t factorRaw =  data[0]       & 0xFF; // B = low byte of 0x0026
                return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
            }
        }
    } else {
        if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_C_COMBINED_REG, 1, data, RS485::RegByteOrder::HighByteSecond)) {
            uint8_t factorRaw = (data[0] >> 8) & 0xFF;     // C = high byte of 0x0027
            return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
        }
    }
    return NAN;
}

/**
 * @brief Read system (combined) power factor.
 * @return Power factor (0.00–1.00), or NAN on failure.
 * @note Register: 0x0027 (low byte = combined PF). Resolution: 0.01/LSB.
 */
float PZEM6L24::readPowerFactor() {
    uint16_t data[1];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_C_COMBINED_REG, 1, data, RS485::RegByteOrder::HighByteSecond)) {
        uint8_t factorRaw = data[0] & 0xFF; // low byte = combined PF (0x0027)
        return factorRaw * PZEM_POWER_FACTOR_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read active energy for a single phase.
 * @param phase 0=A, 1=B, 2=C.
 * @return Energy in kilowatt-hours (kWh), or NAN on failure.
 * @note Registers (low, high): A=(0x0028,0x0029), B=(0x002A,0x002B), C=(0x002C,0x002D). Resolution: 0.1 kWh/LSB.
 */
float PZEM6L24::readActiveEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_REG + (phase * 2), 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t energyRaw = combineRegisters(data[1], data[0]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read reactive energy for a single phase.
 * @param phase 0=A, 1=B, 2=C.
 * @return Reactive energy (kvarh if scaled), or NAN on failure.
 * @note Registers (low, high): A=(0x002E,0x002F), B=(0x0030,0x0031), C=(0x0032,0x0033). Resolution: 0.1 kvarh/LSB.
 */
float PZEM6L24::readReactiveEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_REG + (phase * 2), 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t energyRaw = combineRegisters(data[1], data[0]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read apparent energy for a single phase.
 * @param phase 0=A, 1=B, 2=C.
 * @return Apparent energy (kVAh if scaled), or NAN on failure.
 * @note Registers (low, high): A=(0x0034,0x0035), B=(0x0036,0x0037), C=(0x0038,0x0039). Resolution: 0.1 kVAh/LSB.
 */
float PZEM6L24::readApparentEnergy(uint8_t phase) {
    if (phase > 2) return NAN;
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_REG + (phase * 2), 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t energyRaw = combineRegisters(data[1], data[0]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read system active energy (combined A+B+C).
 * @return Energy in kilowatt-hours (kWh), or NAN on failure.
 * @note Registers (low, high): (0x003A, 0x003B). Resolution: 0.1 kWh/LSB.
 */
float PZEM6L24::readActiveEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_COMBINED_REG, 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t energyRaw = combineRegisters(data[1], data[0]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read system reactive energy (combined A+B+C).
 * @return Reactive energy (kvarh if scaled), or NAN on failure.
 * @note Registers (low, high): (0x003C, 0x003D). Resolution: 0.1 kvarh/LSB.
 */
float PZEM6L24::readReactiveEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_COMBINED_REG, 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t energyRaw = combineRegisters(data[1], data[0]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

/**
 * @brief Read system apparent energy (combined A+B+C).
 * @return Apparent energy (kVAh if scaled), or NAN on failure.
 * @note Registers (low, high): (0x003E, 0x003F). Resolution: 0.1 kVAh/LSB.
 */
float PZEM6L24::readApparentEnergy() {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_COMBINED_REG, 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t energyRaw = combineRegisters(data[1], data[0]);
        return energyRaw * PZEM_ENERGY_RESOLUTION;
    }
    return NAN;
}

// =============================================================================
// Multi-value (batch) readers — fill three references at once
// =============================================================================

/**
 * @brief Read voltages for phases A, B, C in one transaction.
 * @param[out] voltageA Voltage of Phase A (V) or NAN on failure.
 * @param[out] voltageB Voltage of Phase B (V) or NAN on failure.
 * @param[out] voltageC Voltage of Phase C (V) or NAN on failure.
 * @note Registers: 0x0000..0x0002, 0.1 V/LSB.
 */
void PZEM6L24::readVoltage(float& voltageA, float& voltageB, float& voltageC) {
    uint16_t data[3];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_REG, 3, data, RS485::RegByteOrder::HighByteSecond)) {
        voltageA = data[0] * PZEM_VOLTAGE_RESOLUTION;
        voltageB = data[1] * PZEM_VOLTAGE_RESOLUTION;
        voltageC = data[2] * PZEM_VOLTAGE_RESOLUTION;
    } else {
        voltageA = voltageB = voltageC = NAN;
    }
}

/**
 * @brief Read currents for phases A, B, C in one transaction.
 * @param[out] currentA Current of Phase A (A) or NAN on failure.
 * @param[out] currentB Current of Phase B (A) or NAN on failure.
 * @param[out] currentC Current of Phase C (A) or NAN on failure.
 * @note Registers: 0x0003..0x0005, 0.01 A/LSB.
 */
void PZEM6L24::readCurrent(float& currentA, float& currentB, float& currentC) {
    uint16_t data[3];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_REG, 3, data, RS485::RegByteOrder::HighByteSecond)) {
        currentA = data[0] * PZEM_CURRENT_RESOLUTION;
        currentB = data[1] * PZEM_CURRENT_RESOLUTION;
        currentC = data[2] * PZEM_CURRENT_RESOLUTION;
    } else {
        currentA = currentB = currentC = NAN;
    }
}

/**
 * @brief Read frequencies for phases A, B, C in one transaction.
 * @param[out] frequencyA Frequency of Phase A (Hz) or NAN on failure.
 * @param[out] frequencyB Frequency of Phase B (Hz) or NAN on failure.
 * @param[out] frequencyC Frequency of Phase C (Hz) or NAN on failure.
 * @note Registers: 0x0006..0x0008, 0.01 Hz/LSB.
 */
void PZEM6L24::readFrequency(float& frequencyA, float& frequencyB, float& frequencyC) {
    uint16_t data[3];
    if (readInputRegisters(_slaveAddr, PZEM_FREQUENCY_REG, 3, data, RS485::RegByteOrder::HighByteSecond)) {
        frequencyA = data[0] * PZEM_FREQUENCY_RESOLUTION;
        frequencyB = data[1] * PZEM_FREQUENCY_RESOLUTION;
        frequencyC = data[2] * PZEM_FREQUENCY_RESOLUTION;
    } else {
        frequencyA = frequencyB = frequencyC = NAN;
    }
}

/**
 * @brief Read active power for phases A, B, C in one transaction.
 * @param[out] powerA Active power Phase A (W) or NAN on failure.
 * @param[out] powerB Active power Phase B (W) or NAN on failure.
 * @param[out] powerC Active power Phase C (W) or NAN on failure.
 * @note Registers: A=(0x000E,0x000F), B=(0x0010,0x0011), C=(0x0012,0x0013); 0.1 W/LSB, signed.
 */
void PZEM6L24::readActivePower(float& powerA, float& powerB, float& powerC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_POWER_REG, 6, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t powerARaw = combineRegisters(data[1], data[0]);
        uint32_t powerBRaw = combineRegisters(data[3], data[2]);
        uint32_t powerCRaw = combineRegisters(data[5], data[4]);
        powerA = (int32_t)powerARaw * PZEM_POWER_RESOLUTION;
        powerB = (int32_t)powerBRaw * PZEM_POWER_RESOLUTION;
        powerC = (int32_t)powerCRaw * PZEM_POWER_RESOLUTION;
    } else {
        powerA = powerB = powerC = NAN;
    }
}

/**
 * @brief Read reactive power for phases A, B, C in one transaction.
 * @param[out] powerA Reactive power Phase A (var) or NAN on failure.
 * @param[out] powerB Reactive power Phase B (var) or NAN on failure.
 * @param[out] powerC Reactive power Phase C (var) or NAN on failure.
 * @note Registers: A=(0x0014,0x0015), B=(0x0016,0x0017), C=(0x0018,0x0019); 0.1 var/LSB, signed.
 */
void PZEM6L24::readReactivePower(float& powerA, float& powerB, float& powerC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_POWER_REG, 6, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t uPowerARaw = combineRegisters(data[1], data[0]);
        uint32_t uPowerBRaw = combineRegisters(data[3], data[2]);
        uint32_t uPowerCRaw = combineRegisters(data[5], data[4]);
        powerA = (int32_t)uPowerARaw * PZEM_POWER_RESOLUTION;
        powerB = (int32_t)uPowerBRaw * PZEM_POWER_RESOLUTION;
        powerC = (int32_t)uPowerCRaw * PZEM_POWER_RESOLUTION;
    } else {
        powerA = powerB = powerC = NAN;
    }
}

/**
 * @brief Read apparent power for phases A, B, C in one transaction.
 * @param[out] powerA Apparent power Phase A (VA) or NAN on failure.
 * @param[out] powerB Apparent power Phase B (VA) or NAN on failure.
 * @param[out] powerC Apparent power Phase C (VA) or NAN on failure.
 * @note Registers: A=(0x001A,0x001B), B=(0x001C,0x001D), C=(0x001E,0x001F); 0.1 VA/LSB, signed.
 */
void PZEM6L24::readApparentPower(float& powerA, float& powerB, float& powerC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_POWER_REG, 6, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t powerARaw = combineRegisters(data[1], data[0]);
        uint32_t powerBRaw = combineRegisters(data[3], data[2]);
        uint32_t powerCRaw = combineRegisters(data[5], data[4]);
        powerA = (int32_t)powerARaw * PZEM_POWER_RESOLUTION;
        powerB = (int32_t)powerBRaw * PZEM_POWER_RESOLUTION;
        powerC = (int32_t)powerCRaw * PZEM_POWER_RESOLUTION;
    } else {
        powerA = powerB = powerC = NAN;
    }
}

/**
 * @brief Read power factor for phases A, B, C in one transaction.
 * @param[out] factorA PF of Phase A (0.00–1.00) or NAN on failure.
 * @param[out] factorB PF of Phase B (0.00–1.00) or NAN on failure.
 * @param[out] factorC PF of Phase C (0.00–1.00) or NAN on failure.
 * @note Registers: 0x0026 (hi=A, lo=B) and 0x0027 (hi=C). Resolution: 0.01/LSB.
 */
void PZEM6L24::readPowerFactor(float& factorA, float& factorB, float& factorC) {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_POWER_FACTOR_A_B_REG, 2, data, RS485::RegByteOrder::HighByteSecond)) {
        uint8_t factorARaw = (data[0] >> 8) & 0xFF; // 0x0026 hi
        uint8_t factorBRaw =  data[0]       & 0xFF; // 0x0026 lo
        uint8_t factorCRaw = (data[1] >> 8) & 0xFF; // 0x0027 hi
        factorA = factorARaw * PZEM_POWER_FACTOR_RESOLUTION;
        factorB = factorBRaw * PZEM_POWER_FACTOR_RESOLUTION;
        factorC = factorCRaw * PZEM_POWER_FACTOR_RESOLUTION;
    } else {
        factorA = factorB = factorC = NAN;
    }
}

/**
 * @brief Read active energy for phases A, B, C in one transaction.
 * @param[out] energyA Energy Phase A (kWh) or NAN on failure.
 * @param[out] energyB Energy Phase B (kWh) or NAN on failure.
 * @param[out] energyC Energy Phase C (kWh) or NAN on failure.
 * @note Registers: A=(0x0028,0x0029), B=(0x002A,0x002B), C=(0x002C,0x002D); 0.1 kWh/LSB.
 */
void PZEM6L24::readActiveEnergy(float& energyA, float& energyB, float& energyC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_ACTIVE_ENERGY_REG, 6, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t energyARaw = combineRegisters(data[1], data[0]);
        uint32_t energyBRaw = combineRegisters(data[3], data[2]);
        uint32_t energyCRaw = combineRegisters(data[5], data[4]);
        energyA = energyARaw * PZEM_ENERGY_RESOLUTION;
        energyB = energyBRaw * PZEM_ENERGY_RESOLUTION;
        energyC = energyCRaw * PZEM_ENERGY_RESOLUTION;
    } else {
        energyA = energyB = energyC = NAN;
    }
}

/**
 * @brief Read reactive energy for phases A, B, C in one transaction.
 * @param[out] energyA Reactive energy Phase A (kvarh if scaled) or NAN on failure.
 * @param[out] energyB Reactive energy Phase B (kvarh if scaled) or NAN on failure.
 * @param[out] energyC Reactive energy Phase C (kvarh if scaled) or NAN on failure.
 * @note Registers: A=(0x002E,0x002F), B=(0x0030,0x0031), C=(0x0032,0x0033); 0.1 kvarh/LSB.
 */
void PZEM6L24::readReactiveEnergy(float& energyA, float& energyB, float& energyC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_REACTIVE_ENERGY_REG, 6, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t energyARaw = combineRegisters(data[1], data[0]);
        uint32_t energyBRaw = combineRegisters(data[3], data[2]);
        uint32_t energyCRaw = combineRegisters(data[5], data[4]);
        energyA = energyARaw * PZEM_ENERGY_RESOLUTION;
        energyB = energyBRaw * PZEM_ENERGY_RESOLUTION;
        energyC = energyCRaw * PZEM_ENERGY_RESOLUTION;
    } else {
        energyA = energyB = energyC = NAN;
    }
}

/**
 * @brief Read apparent energy for phases A, B, C in one transaction.
 * @param[out] energyA Apparent energy Phase A (kVAh if scaled) or NAN on failure.
 * @param[out] energyB Apparent energy Phase B (kVAh if scaled) or NAN on failure.
 * @param[out] energyC Apparent energy Phase C (kVAh if scaled) or NAN on failure.
 * @note Registers: A=(0x0034,0x0035), B=(0x0036,0x0037), C=(0x0038,0x0039); 0.1 kVAh/LSB.
 */
void PZEM6L24::readApparentEnergy(float& energyA, float& energyB, float& energyC) {
    uint16_t data[6];
    if (readInputRegisters(_slaveAddr, PZEM_APPARENT_ENERGY_REG, 6, data, RS485::RegByteOrder::HighByteSecond)) {
        uint32_t energyARaw = combineRegisters(data[1], data[0]);
        uint32_t energyBRaw = combineRegisters(data[3], data[2]);
        uint32_t energyCRaw = combineRegisters(data[5], data[4]);
        energyA = energyARaw * PZEM_ENERGY_RESOLUTION;
        energyB = energyBRaw * PZEM_ENERGY_RESOLUTION;
        energyC = energyCRaw * PZEM_ENERGY_RESOLUTION;
    } else {
        energyA = energyB = energyC = NAN;
    }
}

/**
 * @brief Read voltage phase angles for A, B, C in one transaction.
 * @param[out] angleA Angle of Phase A (deg). A is reference (0.0f if ok), NAN on failure.
 * @param[out] angleB Angle of Phase B (deg) or NAN on failure.
 * @param[out] angleC Angle of Phase C (deg) or NAN on failure.
 * @note Registers: B=0x0009, C=0x000A. Resolution: 0.01°/LSB relative to A.
 */
void PZEM6L24::readVoltagePhaseAngle(float& angleA, float& angleB, float& angleC) {
    uint16_t data[2];
    if (readInputRegisters(_slaveAddr, PZEM_VOLTAGE_PHASE_REG, 2, data, RS485::RegByteOrder::HighByteSecond)) {
        angleA = 0.0f; // Phase A is reference
        angleB = data[0] * PZEM_PHASE_RESOLUTION;
        angleC = data[1] * PZEM_PHASE_RESOLUTION;
    } else {
        angleA = angleB = angleC = NAN;
    }
}

/**
 * @brief Read current phase angles for A, B, C in one transaction.
 * @param[out] angleA Angle of Phase A (deg) or NAN on failure.
 * @param[out] angleB Angle of Phase B (deg) or NAN on failure.
 * @param[out] angleC Angle of Phase C (deg) or NAN on failure.
 * @note Registers: A=0x000B, B=0x000C, C=0x000D. Resolution: 0.01°/LSB.
 */
void PZEM6L24::readCurrentPhaseAngle(float& angleA, float& angleB, float& angleC) {
    uint16_t data[3];
    if (readInputRegisters(_slaveAddr, PZEM_CURRENT_PHASE_REG, 3, data, RS485::RegByteOrder::HighByteSecond)) {
        angleA = data[0] * PZEM_PHASE_RESOLUTION;
        angleB = data[1] * PZEM_PHASE_RESOLUTION;
        angleC = data[2] * PZEM_PHASE_RESOLUTION;
    } else {
        angleA = angleB = angleC = NAN;
    }
}

/**
 * @brief Reset energy counters.
 * @param phaseOption Bitmask or enum understood by the device (e.g., ALL, or per-phase).
 * @return true on success, false on failure.
 */
bool PZEM6L24::resetEnergy(uint8_t phaseOption) {
    return RS485::resetEnergy(_slaveAddr, phaseOption);
}
