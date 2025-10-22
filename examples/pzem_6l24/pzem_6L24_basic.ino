/*
 * PZEM-6L24 Three-Phase Monitor (ASCII tables + NAN-aware)
 *
 * - Configurable output serial (OUT)
 */

#include <Arduino.h>
#include "PZEM6L24.h"       // your NAN-based driver

//======================================================================================
// CONFIG: set which serials/ports to use
//======================================================================================

// -------- Output serial (where measurements print) --------
#define OUT_SERIAL        Serial        // change to Serial1/Serial2 if you prefer


// -------- PZEM serial port & pins --------
// AVR: uses SoftwareSerial on pins below
// ESP32: uses HardwareSerial(PZEM_UART_NUM) and pins PZEM_RX/PZEM_TX
#if defined(__AVR_ATmega328P__)
// Example: Arduino UNO/Nano-class (change pins as wired)
static const uint8_t PZEM_RX = 5;       // D5 RX from PZEM (RO)
static const uint8_t PZEM_TX = 4;       // D4 TX to PZEM (DI)
SoftwareSerial PZEM_SERIAL(PZEM_RX, PZEM_TX);
PZEM6L24 pzem(PZEM_SERIAL, 0xF8);
#else
// ESP32-class defaults (change as needed)
static const int PZEM_UART_NUM = 1;     // UART1
static const int PZEM_RX = 5;           // GPIO5  (PZEM TX -> ESP32 RX)
static const int PZEM_TX = 4;           // GPIO4  (PZEM RX -> ESP32 TX)
HardwareSerial PZEM_SERIAL(PZEM_UART_NUM);
PZEM6L24 pzem(PZEM_SERIAL, PZEM_RX, PZEM_TX, 0xF8);
#endif

//======================================================================================
// Aliases for readability
//======================================================================================
#define OUT  OUT_SERIAL


//======================================================================================
// Timing
//======================================================================================
static unsigned long lastVoltageMillis = 0;
static const unsigned long MEASUREMENT_INTERVAL = 10UL * 1000UL; // 10s


//======================================================================================
// Setup / Loop
//======================================================================================
void setup() {
  // Bring up serials
  OUT.begin(115200);
  while (!OUT) { /* wait for native USB */ }

#if defined(__AVR_ATmega328P__)
  PZEM_SERIAL.begin(9600);
#else
  PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);
#endif

  // Init driver
  pzem.begin(9600);

  OUT.println(F("PZEM-6L24 started"));
  OUT.println(F("Waiting for measurements..."));
}

void loop() {
  const unsigned long now = millis();


  if (now - lastVoltageMillis >= MEASUREMENT_INTERVAL) {
    lastVoltageMillis = now;

    OUT.println();
    OUT.println(F("=== PZEM-6L24 Three-Phase Measurements ==="));

    // 1) Individual Phase Measurements
    OUT.println();
    OUT.println(F("1. Single Phase Measurements:"));
    for (int i = 0; i < 3; i++) {
        const float v = pzem.readVoltage(i);
        const float c = pzem.readCurrent(i);
        const float f = pzem.readFrequency(i);
        OUT.print("  Phase ");; OUT.print((char)('A' + i)); OUT.print(": ");
        OUT.print(v); OUT.print(" V, ");
        OUT.print(c); OUT.print(" A, ");
        OUT.print(f); OUT.println(" Hz");
    }
    
    delay(500);

    // 2) Power Measurements
    OUT.println();
    OUT.println(F("3. Power Measurements:"));
    {
      float pA, pB, pC, qA, qB, qC, sA, sB, sC;
      pzem.readActivePower(pA, pB, pC);
      pzem.readReactivePower(qA, qB, qC);
      pzem.readApparentPower(sA, sB, sC);
      OUT.print("  Active Power: "); OUT.print(pA); OUT.print(" W, "); 
      OUT.print(pB); OUT.print(" W, "); OUT.print(pC); OUT.println(" W");
      OUT.print("  Reactive Power: "); OUT.print(qA); OUT.print(" var, "); 
      OUT.print(qB); OUT.print(" var, "); OUT.print(qC); OUT.println(" var");
      OUT.print("  Apparent Power: "); OUT.print(sA); OUT.print(" VA, "); 
      OUT.print(sB); OUT.print(" VA, "); OUT.print(sC); OUT.println(" VA");
    }

    delay(500);

    // 3) Combined Measurements
    OUT.println();
    OUT.println(F("3. Combined Measurements:"));
    const float p  = pzem.readActivePower();
    const float q  = pzem.readReactivePower();
    const float s  = pzem.readApparentPower();
    const float pf = pzem.readPowerFactor();
    const float e  = pzem.readActiveEnergy();
    OUT.print("  Active Power: ");   OUT.print(p);  OUT.println(" W");
    OUT.print("  Reactive Power: "); OUT.print(q);  OUT.println(" var");
    OUT.print("  Apparent Power: "); OUT.print(s);  OUT.println(" VA");
    OUT.print("  Power Factor: ");   OUT.print(pf); OUT.println("");
    OUT.print("  Active Energy: ");  OUT.print(e);  OUT.println(" kWh");
    
    delay(200);

    OUT.println();
    OUT.println(F("=========================================="));
    OUT.println(F("End of measurement cycle"));
    OUT.println(F("=========================================="));
  }
}
