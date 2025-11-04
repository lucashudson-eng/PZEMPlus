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

    pzem.begin(9600);

    // Configure timeouts
    // pzem.setTimeouts(100); // 100ms timeout to wait for response

    Serial.print("Software hardware settings: ");
    Serial.println(pzem.getSoftwareHardwareSettings());
    Serial.print("Software address: ");
    Serial.println(pzem.getAddress());
    Serial.print("Baudrate: ");
    Serial.println(pzem.getBaudrate());
    Serial.print("Connection type: ");
    Serial.println(pzem.getConnectionType());
    Serial.print("Frequency system: ");
    Serial.println(pzem.getFrequency());

    // Set software address to 0x01 (0x01 to 0xF7)
    // pzem.setAddress(0x01);

    // Set hardware adress usage (default)
    // pzem.setAddress(0x00);

    // Set baudrate and connection type (same register)
    // 2400, 4800, 9600, 19200, 38400, 57600, 115200
    // PZEM_CONNECTION_3PHASE_4WIRE, PZEM_CONNECTION_3PHASE_3WIRE
    // pzem.setBaudrateAndConnectionType(9600, PZEM_CONNECTION_3PHASE_4WIRE);

    // Set frequency system (50Hz or 60Hz)
    // pzem.setFrequency(50);

    // Reset energy counter (PZEM_RESET_ENERGY_A, PZEM_RESET_ENERGY_B, PZEM_RESET_ENERGY_C, PZEM_RESET_ENERGY_COMBINED, PZEM_RESET_ENERGY_ALL)
    // pzem.resetEnergy(PZEM_RESET_ENERGY_ALL);
    
    Serial.println("PZEM-6L24 started");
    Serial.println("Waiting for measurements...");
}

void loop() {
    Serial.println("\n=== PZEM-6L24 Three-Phase Measurements ===");
    Serial.println("Each measurement includes both individual and multi-phase readings. Below are some mixed examples of how to use them.");
    Serial.println("For power and energy values, there are individual, multi-phase, and combined measurements available.");

    // Test 1: Individual Phase Measurements
    Serial.println("\n1. Individual Phase Measurements:");
    Serial.println("┌──────────┬─────────┬─────────┬─────────┐");
    Serial.println("│ Phase    │ Voltage │ Current │  Freq.  │");
    Serial.println("├──────────┼─────────┼─────────┼─────────┤");
    
    for (uint8_t phase = 0; phase < 3; phase++) {
        float voltage = pzem.readVoltage(phase);
        float current = pzem.readCurrent(phase);
        float frequency = pzem.readFrequency(phase);
        
        Serial.printf("│ Phase %c  │", 'A' + phase);
        if (!isnan(voltage)) {
            Serial.printf("%7.1f V│", voltage);
        } else {
            Serial.print("  Error  │");
        }
        
        if (!isnan(current)) {
            Serial.printf("%7.2f A│", current);
        } else {
            Serial.print("  Error  │");
        }
        
        if (!isnan(frequency)) {
            Serial.printf("%6.2f Hz│", frequency);
        } else {
            Serial.print("  Error  │");
        }
        Serial.println();
    }
    Serial.println("└──────────┴─────────┴─────────┴─────────┘");
    
    delay(200);
    
    // Test 2: Multi-Phase Voltage Reading
    Serial.println("\n2. Multi-Phase Voltage Reading:");
    float voltage[3];
    pzem.readVoltage(voltage[0], voltage[1], voltage[2]);
    
    Serial.println("┌───────────┬─────────┬─────────┬─────────┐");
    Serial.println("│   Method  │ Phase A │ Phase B │ Phase C │");
    Serial.println("├───────────┼─────────┼─────────┼─────────┤");
    Serial.print("│  Multi V  │");
    for (uint8_t i = 0; i < 3; i++) {
        if (!isnan(voltage[i])) {
            Serial.printf("%7.1f V│", voltage[i]);
        } else {
            Serial.print("  Error  │");
        }
    }
    Serial.println();
    Serial.println("└───────────┴─────────┴─────────┴─────────┘");
    
    delay(200);
    
    // Test 3: Power Measurements
    Serial.println("\n3. Power Measurements:");
    float activePower[3], reactivePower[3], apparentPower[3];
    pzem.readActivePower(activePower[0], activePower[1], activePower[2]);
    pzem.readReactivePower(reactivePower[0], reactivePower[1], reactivePower[2]);
    pzem.readApparentPower(apparentPower[0], apparentPower[1], apparentPower[2]);
    
    Serial.println("┌───────────┬───────────┬───────────┬───────────┐");
    Serial.println("│   Type    │  Phase A  │  Phase B  │  Phase C  │");
    Serial.println("├───────────┼───────────┼───────────┼───────────┤");
    
    // Active Power
    Serial.print("│   Active  │");
    for (uint8_t i = 0; i < 3; i++) {
        if (!isnan(activePower[i])) {
            Serial.printf("%9.1f W│", activePower[i]);
        } else {
            Serial.print("   Error   │");
        }
    }
    Serial.println();
    
    // Reactive Power
    Serial.print("│  Reactive │");
    for (uint8_t i = 0; i < 3; i++) {
        if (!isnan(reactivePower[i])) {
            Serial.printf("%7.1f VAr│", reactivePower[i]);
        } else {
            Serial.print("   Error   │");
        }
    }
    Serial.println();
    
    // Apparent Power
    Serial.print("│  Apparent │");
    for (uint8_t i = 0; i < 3; i++) {
        if (!isnan(apparentPower[i])) {
            Serial.printf("%8.1f VA│", apparentPower[i]);
        } else {
            Serial.print("   Error   │");
        }
    }
    Serial.println();
    Serial.println("└───────────┴───────────┴───────────┴───────────┘");
    
    delay(200);
    
    // Test 4: Power Factor
    Serial.println("\n4. Power Factor Measurements:");
    float factor[3];
    pzem.readPowerFactor(factor[0], factor[1], factor[2]);
    
    Serial.println("┌─────────┬─────────┬─────────┬─────────┐");
    Serial.println("│   Type  │ Phase A │ Phase B │ Phase C │");
    Serial.println("├─────────┼─────────┼─────────┼─────────┤");
    
    Serial.print("│    PF   │");
    for (uint8_t i = 0; i < 3; i++) {
        if (!isnan(factor[i])) {
            Serial.printf("%8.2f │", factor[i]);
        } else {
            Serial.print("  Error  │");
        }
    }
    Serial.println();
    Serial.println("└─────────┴─────────┴─────────┴─────────┘");
    
    delay(200);
    
    // Test 5: Energy Measurements
    Serial.println("\n5. Energy Measurements:");
    float energy[3];
    pzem.readActiveEnergy(energy[0], energy[1], energy[2]);
    
    Serial.println("┌─────────┬───────────────┬───────────────┬───────────────┐");
    Serial.println("│   Type  │    Phase A    │    Phase B    │    Phase C    │");
    Serial.println("├─────────┼───────────────┼───────────────┼───────────────┤");
    
    Serial.print("│  Energy │");
    for (uint8_t i = 0; i < 3; i++) {
        if (!isnan(energy[i])) {
            Serial.printf("%11.1f kWh│", energy[i]);
        } else {
            Serial.print("     Error     │");
        }
    }
    Serial.println();
    Serial.println("└─────────┴───────────────┴───────────────┴───────────────┘");
    
    delay(200);
    
    // Test 6: Combined Measurements
    Serial.println("\n6. Combined Measurements:");
    float combinedActive = pzem.readActivePower();
    float combinedReactive = pzem.readReactivePower();
    float combinedApparent = pzem.readApparentPower();
    float combinedFactor = pzem.readPowerFactor();
    float combinedEnergy = pzem.readActiveEnergy();
    
    Serial.println("┌───────────────┬───────────────┬───────────────┬───────────────┬───────────────┐");
    Serial.println("│   Active Pwr  │  Reactive Pwr │  Apparent Pwr │  Power Factor │     Energy    │");
    Serial.println("├───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤");
    
    Serial.print("│");
    if (!isnan(combinedActive)) {
        Serial.printf("%13.1f W│", combinedActive);
    } else {
        Serial.print("     Error     │");
    }
    
    if (!isnan(combinedReactive)) {
        Serial.printf("%11.1f VAr│", combinedReactive);
    } else {
        Serial.print("     Error     │");
    }
    
    if (!isnan(combinedApparent)) {
        Serial.printf("%12.1f VA│", combinedApparent);
    } else {
        Serial.print("     Error     │");
    }
    
    if (!isnan(combinedFactor)) {
        Serial.printf("%14.2f │", combinedFactor);
    } else {
        Serial.print("     Error     │");
    }
    
    if (!isnan(combinedEnergy)) {
        Serial.printf("%11.1f kWh│", combinedEnergy);
    } else {
        Serial.print("     Error     │");
    }
    Serial.println();
    Serial.println("└───────────────┴───────────────┴───────────────┴───────────────┴───────────────┘");
    
    delay(200);
    
    // Test 7: Phase Angles
    Serial.println("\n7. Phase Angle Measurements:");
    float voltageAngle[3], currentAngle[3];
    pzem.readVoltagePhaseAngle(voltageAngle[0], voltageAngle[1], voltageAngle[2]);
    pzem.readCurrentPhaseAngle(currentAngle[0], currentAngle[1], currentAngle[2]);
    
    Serial.println("┌─────────┬─────────┬─────────┬─────────┐");
    Serial.println("│   Type  │ Phase A │ Phase B │ Phase C │");
    Serial.println("├─────────┼─────────┼─────────┼─────────┤");
    
    // Voltage Phase Angles
    Serial.print("│ V Angle │");
    for (uint8_t i = 0; i < 3; i++) {
        if (!isnan(voltageAngle[i])) {
            Serial.printf("%8.2f°│", voltageAngle[i]);
        } else {
            Serial.print("  Error  │");
        }
    }
    Serial.println();
    
    // Current Phase Angles
    Serial.print("│ I Angle │");
    for (uint8_t i = 0; i < 3; i++) {
        if (!isnan(currentAngle[i])) {
            Serial.printf("%8.2f°│", currentAngle[i]);
        } else {
            Serial.print("  Error  │");
        }
    }
    Serial.println();
    Serial.println("└─────────┴─────────┴─────────┴─────────┘");
    
    Serial.println("\n==========================================");
    
    delay(2000); // Wait 5 seconds before next reading
}