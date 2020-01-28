#include "my_ble.h"
#include "my-pcnt.h"

#define WAIT_MS  10 // Time to wait (in milliseconds) before reporting pulse count reading

void setup() {
  Serial.begin(115200);
  BLEInit();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  int16_t count = 0;                        // 16-bit count register
  pcnt_init_and_start();                    // Start counting pulses
  Serial.printf("\nCounter started\n");
  delay(WAIT_MS);                              
  pcnt_get(&count);
  Serial.printf("\nCurrent counter value :%d\n", count);
  pcnt_clear();
}