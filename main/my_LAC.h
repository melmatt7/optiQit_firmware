/* SECTION: Summary
 *  
 * Use LAC module to control linear actuator
 *
 * Functionality of relevant GPIOs:
 *   - GPIO16 - CLK+,
 *   - GPIO19 - CW+
 *
 * To do this test, you should connect GPIO16 to the CLK+ port of the TB6560 stepper driver
 * and connect GPIO19 to the CW+ port of the TB6560 stepper driver. 
 * Ensure ports CW- and CLK- are grounded. 
 *
 * An interrupt will be triggered when the linear actuation completes.
 */

void LAC_init()
{
    pinMode(16, OUTPUT);

    pinMode(19, OUTPUT);

    digitalWrite(16, LOW);

    digitalWrite(19, LOW);
}

void LAC_forward()
{
    digitalWrite(16, HIGH);

    delayMicroseconds(300);

    digitalWrite(16, LOW);

    delayMicroseconds(300);
}

void LAC_backward()
{
      delay(500);

      if (digitalRead(19) == LOW) {
          digitalWrite(19, HIGH);
      }
      else {
          digitalWrite(19, LOW);
      }
}
