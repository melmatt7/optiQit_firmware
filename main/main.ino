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
#include "my_LAC.h"

#define WAIT_MS 5    // Time to wait (in milliseconds) before reporting pulse count reading
int16_t distance = 0; // Record the number of steps we've taken void

void setup()
{
  /* CONFIG: Printing to Console */
  Serial.begin(115200); // Set Baud to 115200
  BLEInit();
  LAC_init();
}

void loop()
{
//   LAC_move();
//   distance = distance + 1;
//   if (distance == 1000)
//   {
//     //delay(5000);
//     LAC_dir();
//     distance = 0;
//   }

  int16_t count = 0; // 16-bit count register

  // notify changed value
  if (deviceConnected)
  {
    //Serial.printf("\nDevice Connect\n");
    LAC_move();
    distance = distance + 1; // record this step - 1 rotation for 1/16 bridge
    if (distance == 1000)
    {
      if (pcnt_init_and_start() != ESP_OK)
      { // Start counting pulses
        Serial.printf("\nFAILED: pcnt_init_and_start\n");
      }
      else
      {
        Serial.printf("\nCounter started\n");
      }
      int sum = 0;
      for (int i = 0; i < 100; i++)
      {
        pcnt_clear();
        delay(WAIT_MS);
        pcnt_get(&count);

        // Update serial montior for local debugging
        //Serial.printf("\nCurrent counter_0 value :%d", count);

        sum += count;
      }

      Serial.printf("\nAveraged value :%d", sum / 100);
      //digitalWrite(19, LOW);

      LAC_dir();
      distance = 0;

      // Update bluetooth characteristic with count value for web app
      pCharacteristic->setValue((uint8_t *)&value, sum / 100);
      pCharacteristic->notify();

      pcnt_clear();

    }
    // Removed because we already have WAIT_MS delay
    //delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3
  }
  // disconnecting
//  else if (!deviceConnected && oldDeviceConnected)
//  {
//    delay(500);                  // give the bluetooth stack the chance to get things ready
//    pServer->startAdvertising(); // restart advertising
//    Serial.println("start advertising");
//    oldDeviceConnected = deviceConnected;
//  }
//  else if (!deviceConnected)
//  {
//    delay(500);                  // give the bluetooth stack the chance to get things ready
//    pServer->startAdvertising(); // restart advertising
//    Serial.println("Please Connect via bluetooth in the app");
//  }
  // connecting
//  if (deviceConnected && !oldDeviceConnected)
//  {
//    // do stuff here on connecting
//    oldDeviceConnected = deviceConnected;
//  }
}
