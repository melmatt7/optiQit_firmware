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

#define WAIT_MS 50    // Time to wait (in milliseconds) before reporting pulse count reading
int16_t deltaDistance = 0; // Record the number of steps we've taken

void setup()
{
  /* CONFIG: Printing to Console */
  Serial.begin(115200); // Set Baud to 115200
  BLEInit();
  LAC_init();
  if (pcnt_init_and_start() != ESP_OK)
        { // Start counting pulses
          Serial.printf("\nFAILED: pcnt_init_and_start\n");
        }
}

void loop()
{

  int16_t count = 0; // 16-bit count register

  // notify changed value
  if (deviceConnected)
  {
    // Set direction and move
    LAC_dir();
    LAC_move();
    deltaDistance = deltaDistance + 1; // record this step - 1 rotation for 1/16 bridge
    if (deltaDistance == 1000)
    {
      int sum = 0;
      for (int i = 0; i < 100; i++)
      {
        pcnt_clear();
        delay(WAIT_MS);
        pcnt_get(&count);
        sum += count;
      }

      Serial.printf("\nAveraged value :%d", sum / 100);
      
      // Update bluetooth characteristic with count value for web app
      pCharacteristic->setValue((uint8_t *)&value, sum / 100);
      pCharacteristic->notify();

      // Reset deltaDistance to await next 1000 steps
      deltaDistance = 0;
    }
  }
}
