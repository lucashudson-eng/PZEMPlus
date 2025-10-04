// Define the PZEM model to use
#define PZEM_004T
// #define PZEM_006L24
// #define PZEM_IOT_E02
// #define PZEM_003
// #define PZEM_017

// #define PZEM_RX 2
// #define PZEM_TX 3

#ifndef __AVR_ATmega328P__
  #define PZEM_SERIAL Serial1
#else
  #include <SoftwareSerial.h>
  SoftwareSerial PZEM_SERIAL(PZEM_RX, PZEM_TX);
#endif

#include <PZEMPlus.h>

PZEM004T pzem(PZEM_SERIAL);

void setup() {
    Serial.begin(115200);

    #if defined(__AVR_ATmega328P__)
        PZEM_SERIAL.begin(9600);
    #elif defined(PZEM_RX) && defined(PZEM_TX)
        PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);
    #else
        PZEM_SERIAL.begin(9600);
    #endif
}

void loop() {
    
}
