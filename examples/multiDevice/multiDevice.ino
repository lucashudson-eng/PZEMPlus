/*
 * Multi Device Example
 *
 * This example demonstrates how to use the PZEMPlus library with multiple
 * energy monitoring devices, exampling with PZEM-004T. It shows how to create an array of
 * PZEMPlus objects, initialize each device with different addresses, and
 * efficiently read all measurements from all devices using readAll().
 *
 * Author: Lucas Hudson
 * GitHub: https://github.com/lucashudson-eng/PZEMPlus
 *
 * License: GPL-3.0
 */

#define PZEM_004T
// #define PZEM_014
// #define PZEM_016

#include <PZEMPlus.h>

// Number of PZEM devices to use
#define NUM_DEVICES 3

// #define PZEM_RX 2
// #define PZEM_TX 3

#if defined(__AVR_ATmega328P__)
SoftwareSerial PZEM_SERIAL(PZEM_RX, PZEM_TX);
#else
HardwareSerial PZEM_SERIAL(2);
#endif

// Create array of PZEMPlus objects (all using same serial)
PZEMPlus* pzemDevices[NUM_DEVICES];

void setup(){
  Serial.begin(115200);
  
  Serial.println("PZEM-004T Multi Device Example");
  Serial.print("Initializing ");
  Serial.print(NUM_DEVICES);
  Serial.println(" devices...");
  
  // Initialize each device
  for (int i = 0; i < NUM_DEVICES; i++) {
    uint8_t address = i+1;
    Serial.print("Initializing device ");
    Serial.print(i);
    Serial.print(" with address 0x");
    if (address < 16) Serial.print("0");
    Serial.print(address, HEX);
    Serial.println("...");
    
    // Initialize PZEMPlus object with same serial using new
#if defined(PZEM_RX) && defined(PZEM_TX) && !defined(__AVR_ATmega328P__)
    pzemDevices[i] = new PZEMPlus(PZEM_SERIAL, PZEM_RX, PZEM_TX, address);
#else
    pzemDevices[i] = new PZEMPlus(PZEM_SERIAL, address);
#endif

    // Begin communication
    pzemDevices[i]->begin();
    
    // Configure timeouts
    // pzemDevices[i]->setTimeouts(100); // 100ms timeout to wait for response
    
    delay(100); // Small delay between initializations
  }
  
  Serial.println("All devices initialized successfully!");
  Serial.println("Starting measurements...");
  Serial.println("========================");
}

void loop(){
  Serial.println("=== Multi Device ReadAll() Test ===");
  
  // Print table header
  Serial.println("+--------+--------+--------+--------+--------+--------+--------+--------+");
  Serial.println("| Device | Addr   | Voltage| Current| Power  | Energy | Freq   | PF     |");
  Serial.println("|        |        | (V)    | (A)    | (W)    | (Wh)   | (Hz)   |        |");
  Serial.println("+--------+--------+--------+--------+--------+--------+--------+--------+");
  
  // Read all measurements from all devices
  for (int i = 0; i < NUM_DEVICES; i++) {
    uint8_t address = i+1;
    
    // Read all measurements at once
    float voltage, current, power, energy, frequency, powerFactor;
    
    if (pzemDevices[i]->readAll(&voltage, &current, &power, &energy, &frequency, &powerFactor)) {
      
      // Print device data in table format
      Serial.print("| ");
      Serial.print(i);
      Serial.print("      | 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print("   | ");
      Serial.print(voltage, 1);
      Serial.print("   | ");
      Serial.print(current, 3);
      Serial.print("  | ");
      Serial.print(power, 1);
      Serial.print("   | ");
      Serial.print(energy, 0);
      Serial.print("   | ");
      Serial.print(frequency, 1);
      Serial.print("   | ");
      Serial.print(powerFactor, 2);
      Serial.println("  |");
      
    } else {
      // Print error row
      Serial.print("| ");
      Serial.print(i);
      Serial.print("      | 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("   | ERROR  | ERROR  | ERROR  | ERROR  | ERROR  | ERROR  |");
    }
    
    delay(50); // Small delay between devices
  }

  // Print table footer
  Serial.println("+--------+--------+--------+--------+--------+--------+--------+--------+");
  Serial.println("========================");
  
  delay(2000); // Wait 2 seconds before next reading cycle
}
