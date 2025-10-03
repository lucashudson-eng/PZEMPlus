#define PZEM_004T

#define PZEM_RX 7
#define PZEM_TX 6

#ifndef __AVR_ATmega328P__
  #define PZEM_SERIAL Serial1
#else
  #include <SoftwareSerial.h>
  SoftwareSerial PZEM_SERIAL(PZEM_RX, PZEM_TX);
#endif

#include <PZEMPlus.h>

PZEM004T pzem(PZEM_SERIAL, 0x01);

void setup() {
  Serial.begin(115200);

  #if defined(__AVR_ATmega328P__)
    PZEM_SERIAL.begin(9600);
  #elif defined(PZEM_RX) && defined(PZEM_TX)
    PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);
  #else
    PZEM_SERIAL.begin(9600);
  #endif
  
  // Configure debug and timeouts
  // pzem.setDebug(true);
  // pzem.setTimeouts(500); // 500ms timeout
  
  Serial.println("PZEM-004T started");
  Serial.println("Waiting for measurements...");
}

void loop() {
  Serial.println("=== Individual Methods Test ===");
  
  // Test 1: readVoltage() - fastest
  Serial.println("1. readVoltage()...");
  uint32_t startTime = millis();
  float voltage = pzem.readVoltage();
  uint32_t voltageTime = millis() - startTime;
  
  if (voltage >= 0) {
    Serial.print("Voltage: ");
    Serial.print(voltage, 1);
    Serial.print(" V (");
    Serial.print(voltageTime);
    Serial.println("ms)");
  } else {
    Serial.println("Error reading voltage");
  }
  
  delay(200);
  
  // Test 2: readCurrent()
  Serial.println("2. readCurrent()...");
  startTime = millis();
  float current = pzem.readCurrent();
  uint32_t currentTime = millis() - startTime;
  
  if (current >= 0) {
    Serial.print("Current: ");
    Serial.print(current, 3);
    Serial.print(" A (");
    Serial.print(currentTime);
    Serial.println("ms)");
  } else {
    Serial.println("Error reading current");
  }
  
  delay(200);
  
  // Test 3: readPower()
  Serial.println("3. readPower()...");
  startTime = millis();
  float power = pzem.readPower();
  uint32_t powerTime = millis() - startTime;
  
  if (power >= 0) {
    Serial.print("Power: ");
    Serial.print(power, 1);
    Serial.print(" W (");
    Serial.print(powerTime);
    Serial.println("ms)");
  } else {
    Serial.println("Error reading power");
  }
  
  delay(200);
  
  // Test 4: readEnergy()
  Serial.println("4. readEnergy()...");
  startTime = millis();
  float energy = pzem.readEnergy();
  uint32_t energyTime = millis() - startTime;
  
  if (energy >= 0) {
    Serial.print("Energy: ");
    Serial.print(energy, 3);
    Serial.print(" Wh (");
    Serial.print(energyTime);
    Serial.println("ms)");
  } else {
    Serial.println("Error reading energy");
  }
  
  delay(200);
  
  // Test 5: readFrequency()
  Serial.println("5. readFrequency()...");
  startTime = millis();
  float frequency = pzem.readFrequency();
  uint32_t frequencyTime = millis() - startTime;
  
  if (frequency >= 0) {
    Serial.print("Frequency: ");
    Serial.print(frequency, 1);
    Serial.print(" Hz (");
    Serial.print(frequencyTime);
    Serial.println("ms)");
  } else {
    Serial.println("Error reading frequency");
  }
  
  delay(200);
  
  // Test 6: readPowerFactor()
  Serial.println("6. readPowerFactor()...");
  startTime = millis();
  float powerFactor = pzem.readPowerFactor();
  uint32_t powerFactorTime = millis() - startTime;
  
  if (powerFactor >= 0) {
    Serial.print("Power Factor: ");
    Serial.print(powerFactor, 2);
    Serial.print(" (");
    Serial.print(powerFactorTime);
    Serial.println("ms)");
  } else {
    Serial.println("Error reading power factor");
  }
  
  delay(200);
  
  // Test 7: readAlarmStatus()
  Serial.println("7. readAlarmStatus()...");
  startTime = millis();
  bool alarm = pzem.readAlarmStatus();
  uint32_t alarmTime = millis() - startTime;
  
  if (alarm) {
    Serial.print("Status: ALARM ACTIVE (");
    Serial.print(alarmTime);
    Serial.println("ms)");
  } else {
    Serial.print("Status: Normal (");
    Serial.print(alarmTime);
    Serial.println("ms)");
  }
  
  delay(200);
  
  // Test 8: getAlarmThreshold()
  Serial.println("8. getAlarmThreshold()...");
  startTime = millis();
  uint16_t alarmThreshold = pzem.getAlarmThreshold();
  uint32_t alarmThresholdTime = millis() - startTime;
  
  if (alarmThreshold > 0) {
    Serial.print("Alarm Threshold: ");
    Serial.print(alarmThreshold);
    Serial.print(" W (");
    Serial.print(alarmThresholdTime);
    Serial.println("ms)");
  } else {
    Serial.println("Error reading alarm threshold");
  }
  
  delay(200);
  
  // Test 9: getAddress()
  Serial.println("9. getAddress()...");
  startTime = millis();
  uint8_t address = pzem.getAddress();
  uint32_t addressTime = millis() - startTime;
  
  Serial.print("PZEM Address: 0x");
  if (address < 16) Serial.print("0");
  Serial.print(address, HEX);
  Serial.print(" (");
  Serial.print(addressTime);
  Serial.println("ms)");
  
  delay(200);
  
  // Test 10: readAll() - comparison
  Serial.println("10. readAll() - COMPARISON...");
  startTime = millis();
  float voltageAll, currentAll, powerAll, energyAll, frequencyAll, powerFactorAll;
  bool alarmAll;
  
  if (pzem.readAll(&voltageAll, &currentAll, &powerAll, &energyAll, &frequencyAll, &powerFactorAll, &alarmAll)) {
    uint32_t readAllTime = millis() - startTime;
    Serial.print("readAll() - Total time: ");
    Serial.print(readAllTime);
    Serial.println("ms");
    Serial.print("All measurements: ");
    Serial.print(voltageAll, 1); Serial.print("V, ");
    Serial.print(currentAll, 3); Serial.print("A, ");
    Serial.print(powerAll, 1); Serial.print("W, ");
    Serial.print(energyAll, 3); Serial.print("Wh, ");
    Serial.print(frequencyAll, 1); Serial.print("Hz, ");
    Serial.print(powerFactorAll, 2); Serial.println();
    
    // Calculate total time of individual methods
    uint32_t totalIndividualTime = voltageTime + currentTime + powerTime + energyTime + frequencyTime + powerFactorTime;
    Serial.print("Total time individual methods: ");
    Serial.print(totalIndividualTime);
    Serial.println("ms");
    Serial.print("Savings with readAll(): ");
    Serial.print(totalIndividualTime - readAllTime);
    Serial.println("ms");
  } else {
    Serial.println("Error in readAll()");
  }
  
  Serial.println("========================");

  delay(3000);
}