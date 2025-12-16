/*
* PZEM-004T Example
*
* This example demonstrates how to use the PZEMPlus library with PZEM-004T
* energy monitoring device. It shows both individual measurement methods
* and the efficient batch reading method.
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

// #define PZEM_RX 2
// #define PZEM_TX 3

#if defined(__AVR_ATmega328P__)
SoftwareSerial PZEM_SERIAL(PZEM_RX, PZEM_TX);
#else
HardwareSerial PZEM_SERIAL(2);
// EspSoftwareSerial::UART PZEM_SERIAL;
#endif

#if defined(PZEM_RX) && defined(PZEM_TX) && !defined(__AVR_ATmega328P__)
PZEMPlus pzem(PZEM_SERIAL, PZEM_RX, PZEM_TX);
#else
PZEMPlus pzem(PZEM_SERIAL);
#endif

void setup(){
  Serial.begin(115200);

  pzem.begin();

  // Configure timeouts
  // pzem.setTimeouts(100); // 100ms timeout to wait for response

  Serial.print("Address: ");
  Serial.println(pzem.getAddress());
  Serial.print("Power alarm threshold: ");
  Serial.println(pzem.getPowerAlarm());

  // Set address to 0x01 (0x01 to 0xF7)
  // pzem.setAddress(0x01);

  // Set power alarm threshold
  // pzem.setPowerAlarm(2300);

  // Reset energy counter
  // pzem.resetEnergy();

  Serial.println("PZEM-004T started");
  Serial.println("Waiting for measurements...");
}

void loop(){
  Serial.println("=== Individual Methods Test ===");

  // Test 1: readVoltage()
  Serial.println("1. readVoltage()...");
  uint32_t startTime = millis();
  float voltage = pzem.readVoltage();
  uint32_t voltageTime = millis() - startTime;

  if (!isnan(voltage)){
    Serial.print("Voltage: ");
    Serial.print(voltage, 1);
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

  if (!isnan(current)){
    Serial.print("Current: ");
    Serial.print(current, 3);
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

  if (!isnan(power)){
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

  if (!isnan(energy)){
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

  // Test 5: readFrequency()
  Serial.println("5. readFrequency()...");
  startTime = millis();
  float frequency = pzem.readFrequency();
  uint32_t frequencyTime = millis() - startTime;

  if (!isnan(frequency)){
    Serial.print("Frequency: ");
    Serial.print(frequency, 1);
    Serial.print(" Hz (");
    Serial.print(frequencyTime);
    Serial.println("ms)");
  }
  else{
    Serial.println("Error reading frequency");
  }

  delay(200);

  // Test 6: readPowerFactor()
  Serial.println("6. readPowerFactor()...");
  startTime = millis();
  float powerFactor = pzem.readPowerFactor();
  uint32_t powerFactorTime = millis() - startTime;

  if (!isnan(powerFactor)){
    Serial.print("Power Factor: ");
    Serial.print(powerFactor, 2);
    Serial.print(" (");
    Serial.print(powerFactorTime);
    Serial.println("ms)");
  }
  else{
    Serial.println("Error reading power factor");
  }

  delay(200);

  // Test 7: readPowerAlarm()
  Serial.println("7. readPowerAlarm()...");
  startTime = millis();
  bool powerAlarm = pzem.readPowerAlarm();
  uint32_t powerAlarmTime = millis() - startTime;

  Serial.print("Power Alarm: ");
  Serial.print(powerAlarm ? "ACTIVE" : "Inactive");
  Serial.print(" (");
  Serial.print(powerAlarmTime);
  Serial.println("ms)");

  delay(200);

  // Test 10: readAll()
  Serial.println("10. readAll()...");
  startTime = millis();
  float voltageAll, currentAll, powerAll, energyAll, frequencyAll, powerFactorAll;

  if (pzem.readAll(&voltageAll, &currentAll, &powerAll, &energyAll, &frequencyAll, &powerFactorAll)){
    uint32_t readAllTime = millis() - startTime;
    Serial.print("readAll() - Total time: ");
    Serial.print(readAllTime);
    Serial.println("ms");
    Serial.print("All measurements: ");
    Serial.print(voltageAll, 1);
    Serial.print("V, ");
    Serial.print(currentAll, 3);
    Serial.print("A, ");
    Serial.print(powerAll, 1);
    Serial.print("W, ");
    Serial.print(energyAll, 0);
    Serial.print("Wh, ");
    Serial.print(frequencyAll, 1);
    Serial.print("Hz, ");
    Serial.print(powerFactorAll, 2);
    Serial.println();

    // Calculate total time of individual methods
    uint32_t totalIndividualTime = voltageTime + currentTime + powerTime + energyTime + frequencyTime + powerFactorTime;
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

  Serial.println("\n========================");

  delay(2000);
}