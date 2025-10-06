/*
 * PZEM-6L24 Example
 *
 * This example demonstrates how to use the PZEMPlus library with PZEM-6L24
 * three-phase energy monitoring device. It shows both individual measurement 
 * methods and the efficient multi-phase reading methods.
 *
 * Author: Lucas Hudson
 * GitHub: https://github.com/lucashudson-eng/PZEMPlus
 *
 * License: GPL-3.0
 */

 #define PZEM_6L24

 #include <PZEMPlus.h>
 
 // #define PZEM_RX 2
 // #define PZEM_TX 3
 
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
 
 void setup() {
     Serial.begin(115200);
 
     pzem.begin();
 
     // Configure timeouts
     // pzem.setTimeouts(100); // 100ms timeout to wait for response
     
     Serial.println("PZEM-6L24 started");
     Serial.println("Waiting for measurements...");
 }
 
 void loop() {
     Serial.println("\n=== PZEM-6L24 Three-Phase Measurements ===");
     
     // Test 1: Individual Phase Measurements
     Serial.println("\n1. Individual Phase Measurements:");
     Serial.println("┌─────────┬─────────┬─────────┬─────────┐");
     Serial.println("│ Phase   │ Voltage │ Current │ Freq    │");
     Serial.println("├─────────┼─────────┼─────────┼─────────┤");
     
     for (uint8_t phase = 0; phase < 3; phase++) {
         float voltage = pzem.readVoltage(phase);
         float current = pzem.readCurrent(phase);
         float frequency = pzem.readFrequency(phase);
         
         Serial.print("│ Phase ");
         Serial.print((char)('A' + phase));
         Serial.print("  │ ");
         if (voltage >= 0) {
             Serial.print(voltage, 1);
             Serial.print(" V   │ ");
         } else {
             Serial.print("Error  │ ");
         }
         
         if (current >= 0) {
             Serial.print(current, 3);
             Serial.print(" A  │ ");
         } else {
             Serial.print("Error │ ");
         }
         
         if (frequency >= 0) {
             Serial.print(frequency, 1);
             Serial.print(" Hz │");
         } else {
             Serial.print("Error │");
         }
         Serial.println();
     }
     Serial.println("└─────────┴─────────┴─────────┴─────────┘");
     
     delay(200);
     
      // Test 2: Multi-Phase Voltage Reading
      Serial.println("\n2. Multi-Phase Voltage Reading:");
      float voltage[3];
      pzem.readVoltage(voltage[0], voltage[1], voltage[2]);
      
      Serial.println("┌─────────┬─────────┬─────────┬─────────┐");
      Serial.println("│ Method  │ Phase A │ Phase B │ Phase C │");
      Serial.println("├─────────┼─────────┼─────────┼─────────┤");
      Serial.print("│ Multi   │ ");
      for (uint8_t i = 0; i < 3; i++) {
          if (voltage[i] >= 0) {
              Serial.print(voltage[i], 1);
              Serial.print(" V   │ ");
          } else {
              Serial.print("Error  │ ");
          }
      }
      Serial.println();
      Serial.println("└─────────┴─────────┴─────────┴─────────┘");
     
     delay(200);
     
      // Test 3: Power Measurements
      Serial.println("\n3. Power Measurements:");
      float activePower[3], reactivePower[3], apparentPower[3];
      pzem.readActivePower(activePower[0], activePower[1], activePower[2]);
      pzem.readReactivePower(reactivePower[0], reactivePower[1], reactivePower[2]);
      pzem.readApparentPower(apparentPower[0], apparentPower[1], apparentPower[2]);
      
      Serial.println("┌─────────┬─────────┬─────────┬─────────┐");
      Serial.println("│ Type    │ Phase A │ Phase B │ Phase C │");
      Serial.println("├─────────┼─────────┼─────────┼─────────┤");
      
      // Active Power
      Serial.print("│ Active  │ ");
      for (uint8_t i = 0; i < 3; i++) {
          if (activePower[i] >= 0) {
              Serial.print(activePower[i], 1);
              Serial.print(" W   │ ");
          } else {
              Serial.print("Error  │ ");
          }
      }
      Serial.println();
      
      // Reactive Power
      Serial.print("│ Reactive│ ");
      for (uint8_t i = 0; i < 3; i++) {
          if (reactivePower[i] >= 0) {
              Serial.print(reactivePower[i], 1);
              Serial.print(" VAR │ ");
          } else {
              Serial.print("Error  │ ");
          }
      }
      Serial.println();
      
      // Apparent Power
      Serial.print("│ Apparent│ ");
      for (uint8_t i = 0; i < 3; i++) {
          if (apparentPower[i] >= 0) {
              Serial.print(apparentPower[i], 1);
              Serial.print(" VA  │ ");
          } else {
              Serial.print("Error  │ ");
          }
      }
      Serial.println();
      Serial.println("└─────────┴─────────┴─────────┴─────────┘");
     
     delay(200);
     
      // Test 4: Power Factor
      Serial.println("\n4. Power Factor Measurements:");
      float factor[3];
      pzem.readPowerFactor(factor[0], factor[1], factor[2]);
      
      Serial.println("┌─────────┬─────────┬─────────┬─────────┐");
      Serial.println("│ Phase   │ Factor  │ Phase   │ Factor  │");
      Serial.println("├─────────┼─────────┼─────────┼─────────┤");
      
      // Primeira linha: A e B
      Serial.print("│ Phase A │ ");
      if (factor[0] >= 0) {
          Serial.print(factor[0], 3);
          Serial.print("     │ Phase B │ ");
      } else {
          Serial.print("Error  │ Phase B │ ");
      }
      if (factor[1] >= 0) {
          Serial.print(factor[1], 3);
          Serial.println("     │");
      } else {
          Serial.println("Error │");
      }
      
      // Segunda linha: C
      Serial.print("│ Phase C │ ");
      if (factor[2] >= 0) {
          Serial.print(factor[2], 3);
          Serial.println("     │         │         │");
      } else {
          Serial.println("Error │         │         │");
      }
      Serial.println("└─────────┴─────────┴─────────┴─────────┘");
     
     delay(200);
     
      // Test 5: Energy Measurements
      Serial.println("\n5. Energy Measurements:");
      float energy[3];
      pzem.readActiveEnergy(energy[0], energy[1], energy[2]);
      
      Serial.println("┌─────────┬─────────┬─────────┬─────────┐");
      Serial.println("│ Phase   │ Energy  │ Phase   │ Energy  │");
      Serial.println("├─────────┼─────────┼─────────┼─────────┤");
      
      // Primeira linha: A e B
      Serial.print("│ Phase A │ ");
      if (energy[0] >= 0) {
          Serial.print(energy[0], 2);
          Serial.print(" kWh │ Phase B │ ");
      } else {
          Serial.print("Error  │ Phase B │ ");
      }
      if (energy[1] >= 0) {
          Serial.print(energy[1], 2);
          Serial.println(" kWh │");
      } else {
          Serial.println("Error │");
      }
      
      // Segunda linha: C
      Serial.print("│ Phase C │ ");
      if (energy[2] >= 0) {
          Serial.print(energy[2], 2);
          Serial.println(" kWh │         │         │");
      } else {
          Serial.println("Error │         │         │");
      }
      Serial.println("└─────────┴─────────┴─────────┴─────────┘");
     
     delay(200);
     
      // Test 6: Combined Measurements
      Serial.println("\n6. Combined Measurements:");
      float combinedActive = pzem.readActivePower();
      float combinedReactive = pzem.readReactivePower();
      float combinedApparent = pzem.readApparentPower();
      float combinedFactor = pzem.readPowerFactor();
      float combinedEnergy = pzem.readActiveEnergy();
      
      Serial.println("┌─────────────┬─────────┐");
      Serial.println("│ Type        │ Value   │");
      Serial.println("├─────────────┼─────────┤");
      
      Serial.print("│ Active Pwr  │ ");
      if (combinedActive >= 0) {
          Serial.print(combinedActive, 1);
          Serial.println(" W   │");
      } else {
          Serial.println("Error │");
      }
      
      Serial.print("│ Reactive Pwr│ ");
      if (combinedReactive >= 0) {
          Serial.print(combinedReactive, 1);
          Serial.println(" VAR │");
      } else {
          Serial.println("Error │");
      }
      
      Serial.print("│ Apparent Pwr│ ");
      if (combinedApparent >= 0) {
          Serial.print(combinedApparent, 1);
          Serial.println(" VA  │");
      } else {
          Serial.println("Error │");
      }
      
      Serial.print("│ Power Factor│ ");
      if (combinedFactor >= 0) {
          Serial.print(combinedFactor, 3);
          Serial.println("     │");
      } else {
          Serial.println("Error │");
      }
      
      Serial.print("│ Energy      │ ");
      if (combinedEnergy >= 0) {
          Serial.print(combinedEnergy, 2);
          Serial.println(" kWh │");
      } else {
          Serial.println("Error │");
      }
      Serial.println("└─────────────┴─────────┘");
     
     delay(200);
     
      // Test 7: Phase Angles
      Serial.println("\n7. Phase Angle Measurements:");
      float voltageAngle[3], currentAngle[3];
      pzem.readVoltagePhaseAngle(voltageAngle[0], voltageAngle[1], voltageAngle[2]);
      pzem.readCurrentPhaseAngle(currentAngle[0], currentAngle[1], currentAngle[2]);
      
      Serial.println("┌─────────┬─────────┬─────────┬─────────┐");
      Serial.println("│ Type    │ Phase A │ Phase B │ Phase C │");
      Serial.println("├─────────┼─────────┼─────────┼─────────┤");
      
      // Voltage Phase Angles
      Serial.print("│ V Angle │ ");
      for (uint8_t i = 0; i < 3; i++) {
          if (voltageAngle[i] >= 0) {
              Serial.print(voltageAngle[i], 1);
              Serial.print("°    │ ");
          } else {
              Serial.print("Error │ ");
          }
      }
      Serial.println();
      
      // Current Phase Angles
      Serial.print("│ I Angle │ ");
      for (uint8_t i = 0; i < 3; i++) {
          if (currentAngle[i] >= 0) {
              Serial.print(currentAngle[i], 1);
              Serial.print("°    │ ");
          } else {
              Serial.print("Error │ ");
          }
      }
      Serial.println();
      Serial.println("└─────────┴─────────┴─────────┴─────────┘");
     
     delay(200);
     
     // Test 8: Reset Energy Example (commented out)
     Serial.println("\n8. Reset Energy Example:");
     Serial.println("   (Uncomment the code below to test energy reset)");
     Serial.println("   pzem.resetEnergy(PZEM_RESET_ENERGY_ALL);");
     
     // Uncomment to test energy reset
     // Serial.println("   Resetting all energy counters...");
     // if (pzem.resetEnergy(PZEM_RESET_ENERGY_ALL)) {
     //     Serial.println("   Energy reset successful!");
     // } else {
     //     Serial.println("   Energy reset failed!");
     // }
     
     Serial.println("\n==========================================");
     Serial.println("End of measurement cycle");
     Serial.println("==========================================");
     
     delay(5000); // Wait 5 seconds before next reading
 }