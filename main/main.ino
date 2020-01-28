/*
 * PCNT (Pulse counting): Serena Ramley, January 20, 2020
 * 
 * Pin Connections:
 * GPIO4 is Pulse Input PIN (PCNT_INPUT_SIG_IO in my-pcnt.h)
 * This code assumes and external pulse generator is connected to GPIO4 (also connect GND of ESP32 to GND of pulse generator)
 * Alternatively, ESP32 GPIO18 could be configured to 1 kHz pulse (refer to ESP32 documentation)
 * Connect GPIO5 to 3.3 V
 */

#include "my_PCNT.h"
#include "my_ble.h"

#define WAIT_MS  1000 // Time to wait (in milliseconds) before reporting pulse count reading


void setup() {
  /* CONFIG: Printing to Console */
  Serial.begin(115200);                     // Set Baud to 115200
  BLEInit();
}

void loop() {  
  int16_t count_0 = 0;                      // 16-bit count register
  int16_t count_1 = 0;                      // 16-bit count register

  // notify changed value
  if (deviceConnected) {
      pCharacteristic->setValue((uint8_t*)&value, 4);
      pCharacteristic->notify();
      value++;
      delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
      
      if (pcnt_init_and_start() != ESP_OK){     // Start counting pulses
        Serial.printf("\nFAILED: pcnt_init_and_start\n");
      } else {
        Serial.printf("\nCounter started\n");
      }
      delay(WAIT_MS);                              
      pcnt_get(&count_0, &count_1);
      Serial.printf("\nCurrent counter_0 value :%d", count_0);
      Serial.printf("\nCurrent counter_1 value :%d\n", count_1);
      pcnt_clear();
  }
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
  }
}