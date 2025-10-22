#ifndef PZEM6L24_H
#define PZEM6L24_H

/**
 * @file PZEM6L24.h
 * @brief C++ interface for the Peacefair PZEM-6L24 three-phase energy meter.
 *
 * All single-value readers return IEEE-754 NAN on failure; batch readers set
 * their output references to NAN on failure. Register addresses and LSB
 * resolutions are noted per method for quick reference.
 */

#include <Arduino.h>
#include <stdint.h>
#if defined(__AVR_ATmega328P__)
  #include <SoftwareSerial.h>
#else
  #include <HardwareSerial.h>
#endif

#include "RS485.h"




/* ============================================================================
   PZEM-6L24 holding register addresses (base registers) — from datasheet table
   ========================================================================== */
#define PZEM_CFG_ADDR_TYPE_SLAVE_REG   0x0000  // lo: addr type, hi: slave address (1..247)
#define PZEM_CFG_BAUD_MODULE_REG       0x0001  // lo: baud code, hi: module type
#define PZEM_CFG_FREQ_RESERVED_REG     0x0002  // lo: freq system, hi: reserved


/* ============================================================================
   PZEM-6L24 input register addresses (base registers) — from datasheet table
   ========================================================================== */
#define PZEM_VOLTAGE_REG                     0x0000  // A=0x0000, B=0x0001, C=0x0002
#define PZEM_CURRENT_REG                     0x0003  // A=0x0003, B=0x0004, C=0x0005
#define PZEM_FREQUENCY_REG                   0x0006  // A=0x0006, B=0x0007, C=0x0008
#define PZEM_VOLTAGE_PHASE_REG               0x0009  // B=0x0009, C=0x000A (A is reference)
#define PZEM_CURRENT_PHASE_REG               0x000B  // A=0x000B, B=0x000C, C=0x000D
#define PZEM_ACTIVE_POWER_REG                0x000E  // pairs: A(0x000E,0x000F), B(0x0010,0x0011), C(0x0012,0x0013)
#define PZEM_REACTIVE_POWER_REG              0x0014  // pairs: A(0x0014,0x0015), B(0x0016,0x0017), C(0x0018,0x0019)
#define PZEM_APPARENT_POWER_REG              0x001A  // pairs: A(0x001A,0x001B), B(0x001C,0x001D), C(0x001E,0x001F)
#define PZEM_ACTIVE_POWER_COMBINED_REG       0x0020  // (0x0020,0x0021)
#define PZEM_REACTIVE_POWER_COMBINED_REG     0x0022  // (0x0022,0x0023)
#define PZEM_APPARENT_POWER_COMBINED_REG     0x0024  // (0x0024,0x0025)
#define PZEM_POWER_FACTOR_A_B_REG            0x0026  // hi=A, lo=B
#define PZEM_POWER_FACTOR_C_COMBINED_REG     0x0027  // hi=C, lo=combined
#define PZEM_ACTIVE_ENERGY_REG               0x0028  // pairs: A(0x0028,0x0029), B(0x002A,0x002B), C(0x002C,0x002D)
#define PZEM_REACTIVE_ENERGY_REG             0x002E  // pairs: A(0x002E,0x002F), B(0x0030,0x0031), C(0x0032,0x0033)
#define PZEM_APPARENT_ENERGY_REG             0x0034  // pairs: A(0x0034,0x0035), B(0x0036,0x0037), C(0x0038,0x0039)
#define PZEM_ACTIVE_ENERGY_COMBINED_REG      0x003A  // (0x003A,0x003B)
#define PZEM_REACTIVE_ENERGY_COMBINED_REG    0x003C  // (0x003C,0x003D)
#define PZEM_APPARENT_ENERGY_COMBINED_REG    0x003E  // (0x003E,0x003F)

/* ============================================================================
   Resolutions (LSB scaling)
   ========================================================================== */
#define PZEM_VOLTAGE_RESOLUTION              0.1f   ///< 1 LSB = 0.1 V (unsigned)
#define PZEM_CURRENT_RESOLUTION              0.01f  ///< 1 LSB = 0.01 A (unsigned)
#define PZEM_FREQUENCY_RESOLUTION            0.01f  ///< 1 LSB = 0.01 Hz
#define PZEM_POWER_RESOLUTION                0.1f   ///< 1 LSB = 0.1 W / var / VA (signed pairs)
#define PZEM_POWER_FACTOR_RESOLUTION         0.01f  ///< 1 LSB = 0.01
#define PZEM_ENERGY_RESOLUTION               0.1f   ///< 1 LSB = 0.1 kWh / kvarh / kVAh
#define PZEM_PHASE_RESOLUTION                0.01f  ///< 1 LSB = 0.01 degree


/* ============================================================================
   Reset energy options
   ========================================================================== */
#define PZEM_RESET_ENERGY_A                  0x00
#define PZEM_RESET_ENERGY_B                  0x01
#define PZEM_RESET_ENERGY_C                  0x02
#define PZEM_RESET_ENERGY_COMBINED           0x03
#define PZEM_RESET_ENERGY_ALL                0x0F

// Address type (low byte @ 0x0000)
enum class PZEM_AddressType : uint8_t {
  HardwareSwitch = 0, // default (DIP/hardware)
  Software       = 1
};

// Baud code (low byte @ 0x0001)
enum class PZEM_BaudCode : uint8_t {
  B2400   = 0,
  B4800   = 1,
  B9600   = 2,  // default
  B19200  = 3,
  B38400  = 4,
  B57600  = 5,
  B115200 = 6
};

// Module type (high byte @ 0x0001)
enum class PZEM_ModuleType : uint8_t {
  ThreePhaseFourWire = 0, // default
  ThreePhaseThreeWire = 1
};

// Frequency system (low byte @ 0x0002)
enum class PZEM_FreqSystem : uint8_t {
  Hz50 = 0, // default
  Hz60 = 1
};



/**
 * @class PZEM6L24
 * @brief High-level API for PZEM-6L24 over RS-485/Modbus.
 *
 * @note Single-value readers return NAN on failure. Batch readers set all
 *       out-parameters to NAN on failure.
 */
class PZEM6L24 : public RS485 {
public:
    // ------------------------------------------------------------------------
    // Constructors
    // ------------------------------------------------------------------------
#if defined(__AVR_ATmega328P__)
    /**
     * @brief Construct for AVR using SoftwareSerial.
     * @param serial    Reference to a SoftwareSerial instance.
     * @param slaveAddr Device Modbus slave address (default 0xF8).
     */
    PZEM6L24(SoftwareSerial &serial, uint8_t slaveAddr = 0xF8);
#else
    /**
     * @brief Construct for MCU using HardwareSerial (no explicit pins).
     * @param serial    Reference to a HardwareSerial (e.g., Serial1).
     * @param slaveAddr Device Modbus slave address (default 0xF8).
     */
    PZEM6L24(HardwareSerial &serial, uint8_t slaveAddr = 0xF8);

    /**
     * @brief Construct for MCU using HardwareSerial with custom RX/TX pins.
     * @param serial    Reference to a HardwareSerial (e.g., Serial1).
     * @param rxPin     UART RX GPIO connected to PZEM-6L24.
     * @param txPin     UART TX GPIO connected to PZEM-6L24.
     * @param slaveAddr Device Modbus slave address (default 0xF8).
     */
    PZEM6L24(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin, uint8_t slaveAddr = 0xF8);
#endif

    /**
     * @brief Initialize the serial transport and clear buffers.
     * @param baudrate UART baud, typically 9600.
     */
    void begin(uint32_t baudrate = 9600);

    // ------------------------------------------------------------------------
    // Per-phase single-value readers (phase index: 0=A, 1=B, 2=C)
    // ------------------------------------------------------------------------
    /** @brief Voltage (V). @param phase 0..2. @return V or NAN on failure.
     *  @note Registers A=0x0000, B=0x0001, C=0x0002; 0.1 V/LSB. */
    float readVoltage(uint8_t phase);

    /** @brief Current (A). @param phase 0..2. @return A or NAN on failure.
     *  @note Registers A=0x0003, B=0x0004, C=0x0005; 0.01 A/LSB. */
    float readCurrent(uint8_t phase);

    /** @brief Frequency (Hz). @param phase 0..2. @return Hz or NAN on failure.
     *  @note Registers A=0x0006, B=0x0007, C=0x0008; 0.01 Hz/LSB. */
    float readFrequency(uint8_t phase);

    /** @brief Active power (W, signed). @param phase 0..2. @return W or NAN.
     *  @note Pairs A=(0x000E,0x000F), B=(0x0010,0x0011), C=(0x0012,0x0013); 0.1 W/LSB. */
    float readActivePower(uint8_t phase);

    /** @brief Reactive power (var, signed). @param phase 0..2. @return var or NAN.
     *  @note Pairs A=(0x0014,0x0015), B=(0x0016,0x0017), C=(0x0018,0x0019); 0.1 var/LSB. */
    float readReactivePower(uint8_t phase);

    /** @brief Apparent power (VA, signed). @param phase 0..2. @return VA or NAN.
     *  @note Pairs A=(0x001A,0x001B), B=(0x001C,0x001D), C=(0x001E,0x001F); 0.1 VA/LSB. */
    float readApparentPower(uint8_t phase);

    /** @brief Power factor (0.00–1.00). @param phase 0..2. @return PF or NAN.
     *  @note A/B=0x0026 (hi=A, lo=B); C=high byte of 0x0027; 0.01/LSB. */
    float readPowerFactor(uint8_t phase);

    /** @brief Active energy (kWh). @param phase 0..2. @return kWh or NAN.
     *  @note Pairs A=(0x0028,0x0029), B=(0x002A,0x002B), C=(0x002C,0x002D); 0.1 kWh/LSB. */
    float readActiveEnergy(uint8_t phase);

    /** @brief Reactive energy (kvarh). @param phase 0..2. @return kvarh or NAN.
     *  @note Pairs A=(0x002E,0x002F), B=(0x0030,0x0031), C=(0x0032,0x0033); 0.1 kvarh/LSB. */
    float readReactiveEnergy(uint8_t phase);

    /** @brief Apparent energy (kVAh). @param phase 0..2. @return kVAh or NAN.
     *  @note Pairs A=(0x0034,0x0035), B=(0x0036,0x0037), C=(0x0038,0x0039); 0.1 kVAh/LSB. */
    float readApparentEnergy(uint8_t phase);

    /** @brief Voltage phase angle (deg). @param phase 0..2. @return deg or NAN.
     *  @note B=0x0009, C=0x000A relative to A=reference (0°); 0.01°/LSB. */
    float readVoltagePhaseAngle(uint8_t phase);

    /** @brief Current phase angle (deg). @param phase 0..2. @return deg or NAN.
     *  @note A=0x000B, B=0x000C, C=0x000D; 0.01°/LSB. */
    float readCurrentPhaseAngle(uint8_t phase);

    // ------------------------------------------------------------------------
    // Combined single-value readers (A+B+C)
    // ------------------------------------------------------------------------
    /** @brief Combined active power (W). @return W or NAN.
     *  @note (0x0020,0x0021); 0.1 W/LSB (signed). */
    float readActivePower();

    /** @brief Combined reactive power (var). @return var or NAN.
     *  @note (0x0022,0x0023); 0.1 var/LSB (signed). */
    float readReactivePower();

    /** @brief Combined apparent power (VA). @return VA or NAN.
     *  @note (0x0024,0x0025); 0.1 VA/LSB (signed). */
    float readApparentPower();

    /** @brief Combined power factor. @return 0.00–1.00 or NAN.
     *  @note 0x0027 low byte = combined PF; 0.01/LSB. */
    float readPowerFactor();

    /** @brief Combined active energy (kWh). @return kWh or NAN.
     *  @note (0x003A,0x003B); 0.1 kWh/LSB. */
    float readActiveEnergy();

    /** @brief Combined reactive energy (kvarh). @return kvarh or NAN.
     *  @note (0x003C,0x003D); 0.1 kvarh/LSB. */
    float readReactiveEnergy();

    /** @brief Combined apparent energy (kVAh). @return kVAh or NAN.
     *  @note (0x003E,0x003F); 0.1 kVAh/LSB. */
    float readApparentEnergy();

    // ------------------------------------------------------------------------
    // Batch readers (fill outputs; set NANs on failure)
    // ------------------------------------------------------------------------
    void readVoltage(float& voltageA, float& voltageB, float& voltageC);            ///< @note 0x0000..0x0002
    void readCurrent(float& currentA, float& currentB, float& currentC);            ///< @note 0x0003..0x0005
    void readFrequency(float& frequencyA, float& frequencyB, float& frequencyC);    ///< @note 0x0006..0x0008
    void readActivePower(float& powerA, float& powerB, float& powerC);              ///< @note pairs starting 0x000E
    void readReactivePower(float& powerA, float& powerB, float& powerC);            ///< @note pairs starting 0x0014
    void readApparentPower(float& powerA, float& powerB, float& powerC);            ///< @note pairs starting 0x001A
    void readPowerFactor(float& factorA, float& factorB, float& factorC);           ///< @note 0x0026, 0x0027
    void readActiveEnergy(float& energyA, float& energyB, float& energyC);          ///< @note pairs starting 0x0028
    void readReactiveEnergy(float& energyA, float& energyB, float& energyC);        ///< @note pairs starting 0x002E
    void readApparentEnergy(float& energyA, float& energyB, float& energyC);        ///< @note pairs starting 0x0034
    void readVoltagePhaseAngle(float& angleA, float& angleB, float& angleC);        ///< @note B=0x0009, C=0x000A
    void readCurrentPhaseAngle(float& angleA, float& angleB, float& angleC);        ///< @note A=0x000B, B=0x000C, C=0x000D

    /**
     * @brief Reset energy counters.
     * @param phaseOption Device bitmask/enum (e.g., PZEM_RESET_ENERGY_ALL).
     * @return true on success, false on failure.
     */
    bool resetEnergy(uint8_t phaseOption = PZEM_RESET_ENERGY_ALL);

      // ---------------- Low-level config access (raw register packing) ----------------
  /**
   * @brief Read 0x0000 (low: address type, high: slave address).
   * @param addrType  OUT low byte (PZEM_AddressType) 0 = Hardware, 1 = Software
   * @param slaveAddr OUT high byte (1..247)
   * @return true on success
   */
  bool getAddressTypeAndSlave(PZEM_AddressType &addrType, uint8_t &slaveAddr);

  /**
   * @brief Write 0x0000 (low: address type, high: slave address).
   *        If addrType==Software (1), device uses this software slave address.
   *        Updates internal _slaveAddr on success.
   */
  bool setAddressTypeAndSlave(PZEM_AddressType addrType, uint8_t slaveAddr);

  /**
   * @brief Read 0x0001 (low: baud code, high: module type).
   */
  bool getBaudAndModule(PZEM_BaudCode &baud, PZEM_ModuleType &moduleType);

  /**
   * @brief Write 0x0001 (low: baud code, high: module type).
   *        On success, reconfigures the host UART to the new baud.
   */
  bool setBaudAndModule(PZEM_BaudCode baud, PZEM_ModuleType moduleType);

  /**
   * @brief Read 0x0002 (low: frequency system, high: reserved).
   */
  bool getFrequencySystem(PZEM_FreqSystem &freq);

  /**
   * @brief Write 0x0002 (low: frequency system, high: 0).
   */
  bool setFrequencySystem(PZEM_FreqSystem freq);

  // ---------------- Ergonomic helpers ----------------
  bool getSlaveAddress(uint8_t &slave);     // (reads 0x0000)
  bool setSlaveAddress(uint8_t slave, bool forceSoftware = true);

  bool getAddressType(PZEM_AddressType &type); // (reads 0x0000)
  bool setAddressType(PZEM_AddressType type);

  bool getBaud(uint32_t &baud);             // (reads 0x0001)
  bool setBaud(uint32_t baud);              // maps to PZEM_BaudCode, updates UART on success

  bool getModuleType(PZEM_ModuleType &type); // (reads 0x0001)
  bool setModuleType(PZEM_ModuleType type);  // preserves current baud code

  bool getLineFrequency(uint16_t &hz);      // (reads 0x0002) -> 50 or 60
  bool setLineFrequency(uint16_t hz);       // accepts 50 or 60


private:
    uint8_t _slaveAddr;
#if !defined(__AVR_ATmega328P__)
    uint8_t _rxPin;
    uint8_t _txPin;
#endif
};

#endif // PZEM6L24_H
