#define PZEM_004T

#ifndef __AVR_ATmega328P__
  #define SerialPZEM Serial1
#else
  #include <SoftwareSerial.h>
  SoftwareSerial SerialPZEM(2, 3);
#endif

#include <PZEMPlus.h>

PZEMPlus pzem(SerialPZEM);

void setup() {
  
}

void loop() {

}