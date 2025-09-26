#include <Arduino_FreeRTOS.h> // FreeRTOS
/* PRIZM */ 
#include <PRIZM.h>
PRIZM prz;
// TETRIX
/*-------*/


void setup() {
  prz.PrizmBegin();
}

void loop() {
  prz.setRedLED(HIGH);
  delay(1000);
  prz.setRedLED(LOW);
  delay(1000);
}
