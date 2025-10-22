/*
 * PZEM-6L24 Three-Phase Monitor (ASCII tables + NAN-aware)
 *
 * - Configurable output serial (OUT) and host/bridge serial (HOST)
 * - Hex bridge: send "0A,1F,FF" style hex to HOST; raw bytes go to PZEM and hex response comes back
 * - Clean ASCII tables with fixed widths
 * - Uses NAN to flag read errors; tables display "Err" for any NAN
 */

#include <Arduino.h>
#include "PZEM6L24.h"


//======================================================================================
// CONFIGURATION
//======================================================================================

// -------- Output serial (where measurements print) --------
#define OUT_SERIAL   Serial      // Change to Serial1/Serial2 if preferred

// -------- Host/bridge serial (where you type hex for bridge) --------
#define HOST_SERIAL  Serial      // Can be same as OUT_SERIAL or different

#define PZEM_BAUDRATE  9600      // PZEM-6L24 default baudrate

// -------- PZEM serial port & pins --------
// AVR: uses SoftwareSerial; ESP32: uses HardwareSerial(PZEM_UART_NUM)
#if defined(__AVR_ATmega328P__)
  // Example: Arduino UNO/Nano
  static const uint8_t PZEM_RX = 5;   // D5 RX from PZEM (RO)
  static const uint8_t PZEM_TX = 4;   // D4 TX to PZEM (DI)
  SoftwareSerial PZEM_SERIAL(PZEM_RX, PZEM_TX);
  PZEM6L24 pzem(PZEM_SERIAL, 0xF8);
#else
  // ESP32-class defaults (change as needed)
  static const int PZEM_UART_NUM = 1; // UART1
  static const int PZEM_RX = 5;       // GPIO5 (PZEM TX -> ESP32 RX)
  static const int PZEM_TX = 4;       // GPIO4 (PZEM RX -> ESP32 TX)
  HardwareSerial PZEM_SERIAL(PZEM_UART_NUM);
  PZEM6L24 pzem(PZEM_SERIAL, PZEM_RX, PZEM_TX, 0xF8);
#endif

//======================================================================================
// ALIASES & TIMING
//======================================================================================
#define OUT  OUT_SERIAL
#define HOST HOST_SERIAL

static unsigned long lastMeasurementMillis = 0;
static const unsigned long MEASUREMENT_INTERVAL = 10UL * 1000UL; // 10 seconds

//======================================================================================
// FUNCTION: Read and print device parameters
//======================================================================================
void readAndPrintParameters() {
  OUT.println(F("\n=== PZEM-6L24 Parameters ==="));

  // Address Type
  PZEM_AddressType addressType;
  if (pzem.getAddressType(addressType)) {
    OUT.print(F("Address Type: "));
    if (addressType == PZEM_AddressType::HardwareSwitch)
      OUT.println(F("Hardware (DIP Switch)"));
    else
      OUT.println(F("Software"));
  } else {
    OUT.println(F("Failed to read address type"));
  }

  delay(200);

  // Slave Address
  uint8_t slaveAddr;
  if (pzem.getSlaveAddress(slaveAddr)) {
    OUT.print(F("Slave Address: "));
    OUT.println(slaveAddr);
  } else {
    OUT.println(F("Failed to read slave address"));
  }

  delay(200);

  // Baud Rate
  uint32_t baud;
  if (pzem.getBaud(baud)) {
    OUT.print(F("Baud Rate: "));
    OUT.println(baud);
  } else {
    OUT.println(F("Failed to read baud rate"));
  }

  delay(200);

  // Module Type
  PZEM_ModuleType moduleType;
  if (pzem.getModuleType(moduleType)) {
    OUT.print(F("Module Type: "));
    if (moduleType == PZEM_ModuleType::ThreePhaseFourWire)
      OUT.println(F("Three-Phase Four-Wire"));
    else
      OUT.println(F("Three-Phase Three-Wire"));
  } else {
    OUT.println(F("Failed to read module type"));
  }

  delay(200);

  // Line Frequency
  uint16_t hz;
  if (pzem.getLineFrequency(hz)) {
    OUT.print(F("Line Frequency: "));
    OUT.print(hz);
    OUT.println(F(" Hz"));
  } else {
    OUT.println(F("Failed to read line frequency"));
  }

  OUT.println();
  OUT.println(F("=========================================="));
  OUT.println(F("End of parameter readout"));
  OUT.println(F("=========================================="));
}

//======================================================================================
// SETUP
//======================================================================================
void setup() {
  // Initialize serial interfaces
  OUT.begin(115200);
  while (!OUT) { /* Wait for native USB */ }

  if (&HOST != &OUT) {
    HOST.begin(115200);
    while (!HOST) { /* Optional wait for native USB */ }
  }

#if defined(__AVR_ATmega328P__)
  PZEM_SERIAL.begin(PZEM_BAUDRATE);
#else
  PZEM_SERIAL.begin(PZEM_BAUDRATE, SERIAL_8N1, PZEM_RX, PZEM_TX);
#endif

  // Initialize PZEM driver
  pzem.begin(PZEM_BAUDRATE);
  OUT.println(F("Initializing PZEM-6L24..."));
  delay(5000); // Allow PZEM to stabilize

  // Change baud rate test
  OUT.println(F("\nChanging baud rate test..."));
  if (pzem.setBaud(9600)) {
    OUT.println(F("Baud rate changed to 9600"));
  #if !defined(__AVR_ATmega328P__)
      PZEM_SERIAL.updateBaudRate(9600);
  #endif
    pzem.begin(9600);
    OUT.println(F("Reinitialized PZEM at 9600 baud"));
  } else {
    OUT.println(F("Failed to change baud rate"));
  }

  delay(200);

  // Change frequency system test
  OUT.println(F("\nChanging frequency system test..."));
  if (pzem.setFrequencySystem(PZEM_FreqSystem::Hz50)) {
    OUT.println(F("Line frequency set to 50Hz"));
  } else {
    OUT.println(F("Failed to change line frequency"));
  }

  delay(200);

  // Change module type test
  OUT.println(F("\nChanging module type test..."));
  if (pzem.setModuleType(PZEM_ModuleType::ThreePhaseFourWire)) {
    OUT.println(F("Module type set to ThreePhaseFourWire"));
  } else {
    OUT.println(F("Failed to set module type"));
  }

  OUT.println(F("\nPZEM-6L24 started"));
  OUT.println(F("Waiting for measurements..."));


  // Changing addresstype test
  // For my devices this usuallu fails the first time. So we try up to 3 times.
  /*
  for (int i = 0; i < 3; i++) {
    OUT.println("Changing addressing mode test");
    if (pzem.setAddressType (PZEM_AddressType::HardwareSwitch)) {
      OUT.println(F("Set to Hardware Addressing mode"));
      i = 3; // exit loop
    } else {
      OUT.println(F("Failed to set Addressing mode"));
    }
  }
  */

  // Changing slave address test
  // This will change the adresstype from Hardware to Software
  /*
  if (pzem.setSlaveAddress(0x05)) {
    OUT.println(F("Slave address set to 0x05"));
    delay(100);
    readAndPrintParameters();
  } else {
    OUT.println(F("Failed to set slave address"));
  }
  */

  // End of setup
}

//======================================================================================
// LOOP
//======================================================================================
void loop() {
  unsigned long now = millis();

  if (now - lastMeasurementMillis >= MEASUREMENT_INTERVAL) {
    lastMeasurementMillis = now;
    readAndPrintParameters();
  }
}
