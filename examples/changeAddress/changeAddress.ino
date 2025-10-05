/*
 * PZEM Address Change Tool
 *
 * This example demonstrates how to change the address of a PZEM device.
 * It reads the current address and changes it to the desired value defined
 * in NEW_ADDRESS. The tool continuously monitors and changes the address
 * if needed.
 *
 * Author: Lucas Hudson
 * GitHub: https://github.com/lucashudson-eng/PZEMPlus
 *
 * License: GPL-3.0
 */

// Define the PZEM model to use:
#define PZEM_004T
// #define PZEM_003
// #define PZEM_017
// #define PZEM_006L24
// #define PZEM_IOT_E02

#include <PZEMPlus.h>

#define NEW_ADDRESS 0x01

// #define PZEM_RX 2
// #define PZEM_TX 3

// In case of using MAX485 for PZEM-003/017
// #define PZEM_RS485_EN 4

#if defined(__AVR_ATmega328P__)
SoftwareSerial PZEM_SERIAL(PZEM_RX, PZEM_TX);
#else
HardwareSerial PZEM_SERIAL(2);
#endif

#if defined(PZEM_RX) && defined(PZEM_TX) && !defined(__AVR_ATmega328P__)
PZEMPlus pzem(PZEM_SERIAL, PZEM_RX, PZEM_TX);
#else
PZEMPlus pzem(PZEM_SERIAL);
#endif

void setup()
{
  Serial.begin(115200);

  pzem.begin();

#if defined(PZEM_RS485_EN) && (defined(PZEM_003) || defined(PZEM_017))
  pzem.setEnable(PZEM_RS485_EN);
#endif
}

void loop()
{
  Serial.println("=== PZEM Address Change Tool ===");
  
  // Read current device address
  uint8_t currentAddress = pzem.getAddress();
  
  if (currentAddress == 0xF8) {
    Serial.println("Error: Could not read current device address");
    Serial.println("Check connections and try again");
    delay(5000);
    return;
  }
  
  Serial.print("Current device address: 0x");
  Serial.println(currentAddress, HEX);
  Serial.print("Desired new address: 0x");
  Serial.println(NEW_ADDRESS, HEX);
  
  // Check if address is already the desired one
  if (currentAddress == NEW_ADDRESS) {
    Serial.println("Device is already configured with the desired address!");
    delay(5000);
    return;
  }
  
  // Change address
  Serial.println("Changing address...");
  bool success = pzem.setAddress(NEW_ADDRESS);
  
  if (success) {
    Serial.println("Address changed successfully!");
    
    // Verify new address
    delay(1000); // Wait a bit for device to process
    uint8_t newAddress = pzem.getAddress();
    
    if (newAddress == NEW_ADDRESS) {
      Serial.print("Confirmation: New address is 0x");
      Serial.println(newAddress, HEX);
    } else {
      Serial.println("Warning: Could not confirm address change");
    }
  } else {
    Serial.println("Error: Failed to change address");
    Serial.println("Check if address is valid (0x01 to 0xF7)");
  }
  
  Serial.println("Waiting 5 seconds before next check...");
  delay(5000);
}
