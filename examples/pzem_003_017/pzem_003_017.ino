/*
 * PZEM-003/017 Example
 *
 * This example demonstrates how to use the PZEMPlus library with PZEM-003/017
 * energy monitoring device. It shows both individual measurement methods
 * and the efficient batch reading method.
 *
 * Author: Lucas Hudson
 * GitHub: https://github.com/lucashudson-eng/PZEMPlus
 *
 * License: GPL-3.0
 */

// #define PZEM_003
#define PZEM_017

#include <PZEMPlus.h>

// #define PZEM_RX 2
// #define PZEM_TX 3

// In case of using MAX485
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

void setup(){
  Serial.begin(115200);

  pzem.begin();

#if defined(PZEM_RS485_EN)
  pzem.setEnable(PZEM_RS485_EN);
#endif

  // Set address to 0x01
  // pzem.setAddress(0x01);

  // Set current range to 50, 100, 200 or 300A (PZEM-017 only)
  // pzem.setCurrentRange(300);

  // Set low and high voltage alarm thresholds
  // pzem.setLowVoltageAlarm(7);
  // pzem.setHighVoltageAlarm(300);

  // Reset energy counter
  // pzem.resetEnergy();

  // Configure timeouts
  // pzem.setTimeouts(100); // 100ms timeout to wait for response

  // Configure sample time
  // pzem.setSampleTime(1000); // 1 second sample time for caching
  // pzem.setSampleTime(0);    // Default, disable sample time (always request fresh data)

  Serial.println("PZEM-003/017 started");
  Serial.println("Waiting for measurements...");
}

void loop(){
  Serial.println("=== Individual Methods Test ===");

  // Test 1: readVoltage()
  Serial.println("1. readVoltage()...");
  uint32_t startTime = millis();
  float voltage = pzem.readVoltage();
  uint32_t voltageTime = millis() - startTime;

  if (voltage >= 0){
    Serial.print("Voltage: ");
    Serial.print(voltage, 2);
    Serial.print(" V (");
    Serial.print(voltageTime);
    Serial.println("ms)");
  }
  else{
    Serial.println("Error reading voltage");
  }

  delay(200);

  // Test 2: readCurrent()
  Serial.println("2. readCurrent()...");
  startTime = millis();
  float current = pzem.readCurrent();
  uint32_t currentTime = millis() - startTime;

  if (current >= 0){
    Serial.print("Current: ");
    Serial.print(current, 2);
    Serial.print(" A (");
    Serial.print(currentTime);
    Serial.println("ms)");
  }
  else{
    Serial.println("Error reading current");
  }

  delay(200);

  // Test 3: readPower()
  Serial.println("3. readPower()...");
  startTime = millis();
  float power = pzem.readPower();
  uint32_t powerTime = millis() - startTime;

  if (power >= 0){
    Serial.print("Power: ");
    Serial.print(power, 1);
    Serial.print(" W (");
    Serial.print(powerTime);
    Serial.println("ms)");
  }
  else{
    Serial.println("Error reading power");
  }

  delay(200);

  // Test 4: readEnergy()
  Serial.println("4. readEnergy()...");
  startTime = millis();
  float energy = pzem.readEnergy();
  uint32_t energyTime = millis() - startTime;

  if (energy >= 0){
    Serial.print("Energy: ");
    Serial.print(energy, 0);
    Serial.print(" Wh (");
    Serial.print(energyTime);
    Serial.println("ms)");
  }
  else{
    Serial.println("Error reading energy");
  }

  delay(200);

  // Test 5: readHighVoltageAlarm()
  Serial.println("5. readHighVoltageAlarm()...");
  startTime = millis();
  bool highVoltageAlarm = pzem.readHighVoltageAlarm();
  uint32_t highVoltageAlarmTime = millis() - startTime;

  Serial.print("High Voltage Alarm: ");
  Serial.print(highVoltageAlarm ? "ACTIVE" : "Inactive");
  Serial.print(" (");
  Serial.print(highVoltageAlarmTime);
  Serial.println("ms)");

  delay(200);

  // Test 6: readLowVoltageAlarm()
  Serial.println("6. readLowVoltageAlarm()...");
  startTime = millis();
  bool lowVoltageAlarm = pzem.readLowVoltageAlarm();
  uint32_t lowVoltageAlarmTime = millis() - startTime;

  Serial.print("Low Voltage Alarm: ");
  Serial.print(lowVoltageAlarm ? "ACTIVE" : "Inactive");
  Serial.print(" (");
  Serial.print(lowVoltageAlarmTime);
  Serial.println("ms)");

  delay(200);

  // Test 7: getHighVoltageAlarm()
  Serial.println("7. getHighVoltageAlarm()...");
  startTime = millis();
  float highVoltageThreshold = pzem.getHighVoltageAlarm();
  uint32_t highVoltageThresholdTime = millis() - startTime;

  if (highVoltageThreshold >= 0){
    Serial.print("High Voltage Alarm Threshold: ");
    Serial.print(highVoltageThreshold, 2);
    Serial.print(" V (");
    Serial.print(highVoltageThresholdTime);
    Serial.println("ms)");
  }
  else{
    Serial.println("Error reading high voltage alarm threshold");
  }

  delay(200);

  // Test 8: getLowVoltageAlarm()
  Serial.println("8. getLowVoltageAlarm()...");
  startTime = millis();
  float lowVoltageThreshold = pzem.getLowVoltageAlarm();
  uint32_t lowVoltageThresholdTime = millis() - startTime;

  if (lowVoltageThreshold >= 0){
    Serial.print("Low Voltage Alarm Threshold: ");
    Serial.print(lowVoltageThreshold, 2);
    Serial.print(" V (");
    Serial.print(lowVoltageThresholdTime);
    Serial.println("ms)");
  }
  else{
    Serial.println("Error reading low voltage alarm threshold");
  }

  delay(200);

  // Test 9: getAddress()
  Serial.println("9. getAddress()...");
  startTime = millis();
  uint8_t address = pzem.getAddress();
  uint32_t addressTime = millis() - startTime;

  Serial.print("PZEM Address: 0x");
  if (address < 16)
    Serial.print("0");
  Serial.print(address, HEX);
  Serial.print(" (");
  Serial.print(addressTime);
  Serial.println("ms)");

  delay(200);

  // Test 10: getCurrentRange()
  Serial.println("10. getCurrentRange()...");
  startTime = millis();
  uint16_t currentRange = pzem.getCurrentRange();
  uint32_t currentRangeTime = millis() - startTime;

  Serial.print("Current Range: ");
  Serial.print(currentRange);
  Serial.print(" A (");
  Serial.print(currentRangeTime);
  Serial.println("ms)");

  delay(200);

  // Test 11: readAll()
  Serial.println("11. readAll()...");
  startTime = millis();
  float voltageAll, currentAll, powerAll, energyAll;

  if (pzem.readAll(&voltageAll, &currentAll, &powerAll, &energyAll)){
    uint32_t readAllTime = millis() - startTime;
    Serial.print("readAll() - Total time: ");
    Serial.print(readAllTime);
    Serial.println("ms");
    Serial.print("All measurements: ");
    Serial.print(voltageAll, 2);
    Serial.print("V, ");
    Serial.print(currentAll, 2);
    Serial.print("A, ");
    Serial.print(powerAll, 1);
    Serial.print("W, ");
    Serial.print(energyAll, 0);
    Serial.println("Wh");

    // Calculate total time of individual methods
    uint32_t totalIndividualTime = voltageTime + currentTime + powerTime + energyTime;
    Serial.print("Total time individual methods: ");
    Serial.print(totalIndividualTime);
    Serial.println("ms");
    Serial.print("Savings with readAll(): ");
    Serial.print(totalIndividualTime - readAllTime);
    Serial.println("ms");
  }
  else{
    Serial.println("Error in readAll()");
  }

  Serial.println("========================");

  delay(1000);
}
