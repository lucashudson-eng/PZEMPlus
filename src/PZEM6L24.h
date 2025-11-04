/**
 * @file PZEM6L24.h
 * @brief PZEM-6L24 three-phase energy monitoring module class
 * @author Lucas Hudson
 * @date 2025
 */

#ifndef PZEM6L24_H
#define PZEM6L24_H

#include "RS485.h"

/**
 * @defgroup PZEM6L24Registers PZEM-6L24 Register Addresses
 * @brief Input register addresses for three-phase measurements
 * @{
 */
// Input register addresses
#define PZEM_VOLTAGE_REG                  0x0000 // A=0x0000, B=0x0001, C=0x0002
#define PZEM_CURRENT_REG                  0x0003 // A=0x0003, B=0x0004, C=0x0005
#define PZEM_FREQUENCY_REG                0x0006 // A=0x0006, B=0x0007, C=0x0008
#define PZEM_VOLTAGE_PHASE_REG            0x0009 // B=0x0009, C=0x000A (A is reference)
#define PZEM_CURRENT_PHASE_REG            0x000B // A=0x000B, B=0x000C, C=0x000D
#define PZEM_ACTIVE_POWER_REG             0x000E // A=0x000E/0x000F, B=0x0010/0x0011, C=0x0012/0x0013
#define PZEM_REACTIVE_POWER_REG           0x0014 // A=0x0014/0x0015, B=0x0016/0x0017, C=0x0018/0x0019
#define PZEM_APPARENT_POWER_REG           0x001A // A=0x001A/0x001B, B=0x001C/0x001D, C=0x001E/0x001F
#define PZEM_ACTIVE_POWER_COMBINED_REG    0x0020 // combined=0x0020/0x0021
#define PZEM_REACTIVE_POWER_COMBINED_REG  0x0022 // combined=0x0022/0x0023
#define PZEM_APPARENT_POWER_COMBINED_REG  0x0024 // combined=0x0024/0x0025
#define PZEM_POWER_FACTOR_A_B_REG         0x0026 // A=hi, B=lo
#define PZEM_POWER_FACTOR_C_COMBINED_REG  0x0027 // C=hi, combined=lo
#define PZEM_ACTIVE_ENERGY_REG            0x0028 // A=0x0028/0x0029, B=0x002A/0x002B, C=0x002C/0x002D
#define PZEM_REACTIVE_ENERGY_REG          0x002E // A=0x002E/0x002F, B=0x0030/0x0031, C=0x0032/0x0033
#define PZEM_APPARENT_ENERGY_REG          0x0034 // A=0x0034/0x0035, B=0x0036/0x0037, C=0x0038/0x0039
#define PZEM_ACTIVE_ENERGY_COMBINED_REG   0x003A // combined=0x003A/0x003B
#define PZEM_REACTIVE_ENERGY_COMBINED_REG 0x003C // combined=0x003C/0x003D
#define PZEM_APPARENT_ENERGY_COMBINED_REG 0x003E ///< Apparent energy combined register address

// Parameter registers
#define PZEM_ADDRESS_REG            0x0000  ///< Address register (addr=hi, addr type=lo)
#define PZEM_BAUDRATE_TYPE_REG      0x0001  ///< Baudrate/connection type register (connection type=hi, baudrate=lo)
#define PZEM_FREQUENCY_SYSTEM_REG   0x0002  ///< Frequency system register (reserved=hi, frequency=lo)
/** @} */

/**
 * @defgroup PZEM6L24Resolutions PZEM-6L24 Resolutions
 * @brief Resolution factors for converting raw register values to physical units
 * @{
 */
#define PZEM_VOLTAGE_RESOLUTION      0.1f
#define PZEM_CURRENT_RESOLUTION      0.01f
#define PZEM_FREQUENCY_RESOLUTION    0.01f
#define PZEM_POWER_RESOLUTION        0.1f
#define PZEM_POWER_FACTOR_RESOLUTION 0.01f
#define PZEM_ENERGY_RESOLUTION       0.1f
#define PZEM_PHASE_RESOLUTION        0.01f  ///< Phase angle resolution (degrees per LSB)
/** @} */

/**
 * @defgroup PZEM6L24ResetOptions Reset Energy Options
 * @brief Options for selective energy counter reset
 * @{
 */
#define PZEM_RESET_ENERGY_A        0x00  ///< Reset phase A energy
#define PZEM_RESET_ENERGY_B        0x01  ///< Reset phase B energy
#define PZEM_RESET_ENERGY_C        0x02  ///< Reset phase C energy
#define PZEM_RESET_ENERGY_COMBINED 0x03  ///< Reset combined energy
#define PZEM_RESET_ENERGY_ALL      0x0F  ///< Reset all energy counters
/** @} */

/**
 * @defgroup PZEM6L24Baudrates Baudrate Options
 * @brief Supported baudrate values
 * @{
 */
#define PZEM_BAUDRATE_2400   0x00
#define PZEM_BAUDRATE_4800   0x01
#define PZEM_BAUDRATE_9600   0x02
#define PZEM_BAUDRATE_19200  0x03
#define PZEM_BAUDRATE_38400  0x04
#define PZEM_BAUDRATE_57600  0x05
#define PZEM_BAUDRATE_115200 0x06  ///< 115200 baud
/** @} */

/**
 * @defgroup PZEM6L24ConnectionTypes Connection Type Options
 * @brief Three-phase connection type options
 * @{
 */
#define PZEM_CONNECTION_3PHASE_4WIRE 0x00  ///< 3-phase 4-wire connection
#define PZEM_CONNECTION_3PHASE_3WIRE 0x01  ///< 3-phase 3-wire connection
/** @} */

/**
 * @defgroup PZEM6L24Frequencies Frequency Options
 * @brief AC frequency system options
 * @{
 */
#define PZEM_FREQUENCY_50HZ 0x00  ///< 50 Hz AC system
#define PZEM_FREQUENCY_60HZ 0x01  ///< 60 Hz AC system
/** @} */

/**
 * @class PZEM6L24
 * @brief Class for interfacing with PZEM-6L24 three-phase energy monitoring module
 * 
 * This class provides comprehensive methods to read three-phase electrical measurements
 * including voltage, current, power (active, reactive, apparent), energy, power factor,
 * and phase angles for each phase (A, B, C) as well as combined values.
 */
class PZEM6L24 : public RS485 {
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
    PZEM6L24(SoftwareSerial &serial, uint8_t slaveAddr = 0xF8);
#else
    /**
     * @brief Constructor for ESP32/ESP8266 with HardwareSerial
     * @param serial HardwareSerial object reference
     * @param slaveAddr Slave device address (default: 0xF8)
     */
    PZEM6L24(HardwareSerial &serial, uint8_t slaveAddr = 0xF8);
    
    /**
     * @brief Constructor for ESP32/ESP8266 with HardwareSerial and custom pins
     * @param serial HardwareSerial object reference
     * @param rxPin RX pin number
     * @param txPin TX pin number
     * @param slaveAddr Slave device address (default: 0xF8)
     */
    PZEM6L24(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr = 0xF8);
#endif
    
    /** @} */
    
    /**
     * @brief Initialize serial communication
     * @param baudrate Serial baudrate (default: 9600)
     */
    void begin(uint32_t baudrate = 9600);
    
    /**
     * @name Basic Phase Measurements
     * @brief Read measurements for a specific phase (0=A, 1=B, 2=C)
     * @{
     */
    /**
     * @brief Read voltage for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Voltage in volts, or NAN on error
     */
    float readVoltage(uint8_t phase);
    
    /**
     * @brief Read current for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Current in amperes, or NAN on error
     */
    float readCurrent(uint8_t phase);
    
    /**
     * @brief Read frequency for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Frequency in hertz, or NAN on error
     */
    float readFrequency(uint8_t phase);
    
    /**
     * @brief Read active power for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Active power in watts, or NAN on error
     */
    float readActivePower(uint8_t phase);
    
    /**
     * @brief Read reactive power for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Reactive power in VAR, or NAN on error
     */
    float readReactivePower(uint8_t phase);
    
    /**
     * @brief Read apparent power for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Apparent power in VA, or NAN on error
     */
    float readApparentPower(uint8_t phase);
    
    /**
     * @brief Read power factor for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Power factor (0.00 to 1.00), or NAN on error
     */
    float readPowerFactor(uint8_t phase);
    
    /**
     * @brief Read active energy for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Active energy in watt-hours, or NAN on error
     */
    float readActiveEnergy(uint8_t phase);
    
    /**
     * @brief Read reactive energy for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Reactive energy in VAR-hours, or NAN on error
     */
    float readReactiveEnergy(uint8_t phase);
    
    /**
     * @brief Read apparent energy for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Apparent energy in VA-hours, or NAN on error
     */
    float readApparentEnergy(uint8_t phase);
    
    /**
     * @brief Read voltage phase angle for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Voltage phase angle in degrees, or NAN on error (Phase A is reference at 0°)
     */
    float readVoltagePhaseAngle(uint8_t phase);
    
    /**
     * @brief Read current phase angle for a specific phase
     * @param phase Phase number (0=A, 1=B, 2=C)
     * @return Current phase angle in degrees, or NAN on error
     */
    float readCurrentPhaseAngle(uint8_t phase);
    
    /** @} */
    
    /**
     * @name Combined Measurements
     * @brief Read combined three-phase measurements
     * @{
     */
    /**
     * @brief Read combined active power (all three phases)
     * @return Combined active power in watts, or NAN on error
     */
    float readActivePower();
    
    /**
     * @brief Read combined reactive power (all three phases)
     * @return Combined reactive power in VAR, or NAN on error
     */
    float readReactivePower();
    
    /**
     * @brief Read combined apparent power (all three phases)
     * @return Combined apparent power in VA, or NAN on error
     */
    float readApparentPower();
    
    /**
     * @brief Read combined power factor (all three phases)
     * @return Combined power factor (0.00 to 1.00), or NAN on error
     */
    float readPowerFactor();
    
    /**
     * @brief Read combined active energy (all three phases)
     * @return Combined active energy in watt-hours, or NAN on error
     */
    float readActiveEnergy();
    
    /**
     * @brief Read combined reactive energy (all three phases)
     * @return Combined reactive energy in VAR-hours, or NAN on error
     */
    float readReactiveEnergy();
    
    /**
     * @brief Read combined apparent energy (all three phases)
     * @return Combined apparent energy in VA-hours, or NAN on error
     */
    float readApparentEnergy();
    
    /** @} */
    
    /**
     * @name Multi-Phase Reading Methods
     * @brief Read all three phases simultaneously
     * @{
     */
    /**
     * @brief Read voltage for all three phases simultaneously
     * @param voltageA Reference to store phase A voltage
     * @param voltageB Reference to store phase B voltage
     * @param voltageC Reference to store phase C voltage
     */
    void readVoltage(float& voltageA, float& voltageB, float& voltageC);
    
    /**
     * @brief Read current for all three phases simultaneously
     * @param currentA Reference to store phase A current
     * @param currentB Reference to store phase B current
     * @param currentC Reference to store phase C current
     */
    void readCurrent(float& currentA, float& currentB, float& currentC);
    
    /**
     * @brief Read frequency for all three phases simultaneously
     * @param frequencyA Reference to store phase A frequency
     * @param frequencyB Reference to store phase B frequency
     * @param frequencyC Reference to store phase C frequency
     */
    void readFrequency(float& frequencyA, float& frequencyB, float& frequencyC);
    
    /**
     * @brief Read voltage and current for all three phases simultaneously
     * @param voltageA Reference to store phase A voltage
     * @param voltageB Reference to store phase B voltage
     * @param voltageC Reference to store phase C voltage
     * @param currentA Reference to store phase A current
     * @param currentB Reference to store phase B current
     * @param currentC Reference to store phase C current
     */
    void readVoltageCurrent(float& voltageA, float& voltageB, float& voltageC, float& currentA, float& currentB, float& currentC);
    
    /**
     * @brief Read active power for all three phases simultaneously
     * @param powerA Reference to store phase A active power
     * @param powerB Reference to store phase B active power
     * @param powerC Reference to store phase C active power
     */
    void readActivePower(float& powerA, float& powerB, float& powerC);
    
    /**
     * @brief Read reactive power for all three phases simultaneously
     * @param powerA Reference to store phase A reactive power
     * @param powerB Reference to store phase B reactive power
     * @param powerC Reference to store phase C reactive power
     */
    void readReactivePower(float& powerA, float& powerB, float& powerC);
    
    /**
     * @brief Read apparent power for all three phases simultaneously
     * @param powerA Reference to store phase A apparent power
     * @param powerB Reference to store phase B apparent power
     * @param powerC Reference to store phase C apparent power
     */
    void readApparentPower(float& powerA, float& powerB, float& powerC);
    
    /**
     * @brief Read power factor for all three phases simultaneously
     * @param factorA Reference to store phase A power factor
     * @param factorB Reference to store phase B power factor
     * @param factorC Reference to store phase C power factor
     */
    void readPowerFactor(float& factorA, float& factorB, float& factorC);
    
    /**
     * @brief Read active energy for all three phases simultaneously
     * @param energyA Reference to store phase A active energy
     * @param energyB Reference to store phase B active energy
     * @param energyC Reference to store phase C active energy
     */
    void readActiveEnergy(float& energyA, float& energyB, float& energyC);
    
    /**
     * @brief Read reactive energy for all three phases simultaneously
     * @param energyA Reference to store phase A reactive energy
     * @param energyB Reference to store phase B reactive energy
     * @param energyC Reference to store phase C reactive energy
     */
    void readReactiveEnergy(float& energyA, float& energyB, float& energyC);
    
    /**
     * @brief Read apparent energy for all three phases simultaneously
     * @param energyA Reference to store phase A apparent energy
     * @param energyB Reference to store phase B apparent energy
     * @param energyC Reference to store phase C apparent energy
     */
    void readApparentEnergy(float& energyA, float& energyB, float& energyC);
    
    /**
     * @brief Read voltage phase angle for all three phases simultaneously
     * @param angleA Reference to store phase A voltage angle (always 0° as reference)
     * @param angleB Reference to store phase B voltage angle
     * @param angleC Reference to store phase C voltage angle
     */
    void readVoltagePhaseAngle(float& angleA, float& angleB, float& angleC);
    
    /**
     * @brief Read current phase angle for all three phases simultaneously
     * @param angleA Reference to store phase A current angle
     * @param angleB Reference to store phase B current angle
     * @param angleC Reference to store phase C current angle
     */
    void readCurrentPhaseAngle(float& angleA, float& angleB, float& angleC);
    
    /** @} */
    
    /**
     * @name Parameter Methods
     * @brief Configuration and parameter management
     * @{
     */
    /**
     * @brief Set device slave address
     * @param address New slave address (0x00 for hardware addressing, 0x01-0xF7 for software addressing)
     * @return true if successful, false otherwise
     */
    bool setAddress(uint8_t address = 0x00);
    
    /**
     * @brief Set baudrate and connection type simultaneously
     * @param baudrate Baudrate value in bits per second (2400, 4800, 9600, 19200, 38400, 57600, or 115200)
     * @param connectionType Connection type (PZEM_CONNECTION_3PHASE_4WIRE or PZEM_CONNECTION_3PHASE_3WIRE)
     * @param forceBaudrate If true, change serial baudrate even if write fails
     * @return true if successful, false otherwise
     */
    bool setBaudrateAndConnectionType(uint32_t baudrate = 9600, uint8_t connectionType = PZEM_CONNECTION_3PHASE_4WIRE, bool forceBaudrate = true);
    
    /**
     * @brief Set AC frequency system (50Hz or 60Hz)
     * @param frequency Frequency value in Hz (50 or 60)
     * @return true if successful, false otherwise
     */
    bool setFrequency(uint8_t frequency = 50);
    
    /**
     * @brief Get addressing mode (software or hardware)
     * @return true if using software addressing, false if using hardware addressing
     */
    bool getSoftwareHardwareSettings();
    
    /**
     * @brief Get device slave address
     * @return Current slave address (0-254), or 0xFF on error
     */
    uint8_t getAddress();
    
    /**
     * @brief Get current baudrate setting
     * @return Baudrate in bits per second (2400, 4800, 9600, 19200, 38400, 57600, or 115200), or 0 on error
     */
    uint32_t getBaudrate();
    
    /**
     * @brief Get connection type setting
     * @return Connection type (0=PZEM_CONNECTION_3PHASE_4WIRE, 1=PZEM_CONNECTION_3PHASE_3WIRE), or 0xFF on error
     */
    uint8_t getConnectionType();
    
    /**
     * @brief Get AC frequency system setting
     * @return Frequency in Hz (50 or 60), or 0 on error
     */
    uint8_t getFrequency();
    
    /** @} */
    
    /**
     * @name Control Methods
     * @{
     */
    
    /**
     * @brief Reset energy counter(s)
     * @param phaseOption Reset option (PZEM_RESET_ENERGY_A, PZEM_RESET_ENERGY_B, PZEM_RESET_ENERGY_C, PZEM_RESET_ENERGY_COMBINED, or PZEM_RESET_ENERGY_ALL)
     * @return true if successful, false otherwise
     */
    bool resetEnergy(uint8_t phaseOption = PZEM_RESET_ENERGY_ALL);

    /** @} */

private:
    uint8_t _slaveAddr;  ///< Current slave device address
#if !defined(__AVR_ATmega328P__)
    uint8_t _rxPin;      ///< RX pin number (-1 if not used)
    uint8_t _txPin;      ///< TX pin number (-1 if not used)
#endif
};

#endif // PZEM6L24_H