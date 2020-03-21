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
 *
 * For the NEMA23 RTM23HS6430 2-Phase Stepper motor (compatible with TB6560 stepper driver),
 * 1 step is (screw pitch) * (step angle) / 360 degrees
 * For the NEMA23, this is 5 mm * 1.6 degrees/360 degrees = 0.02 mm, based on datasheet
 * 1000 steps is 22 mm, for this particular motor.
 */

// Example implementation:
//   LAC_dir();
//   LAC_move();
//   deltaDistance = deltaDistance + 1;
//   if (deltaDistance == 1000)
//   {
//     Put actions here
//     deltaDistance = 0;
//   }

#define CLK 16
#define CW 19

/*
 * Set CLK (determines when the actuator will move) to output
 * Set CW (determines in which direction the actuator will move) to output
*/
void LAC_init()
{
    pinMode(CLK, OUTPUT);

    pinMode(CW, OUTPUT);

    digitalWrite(CLK, LOW);

    digitalWrite(CW, LOW);
}

/*
 * Every time a pulse is sent to the CLK, the actuator moves one step.
*/
void LAC_move()
{
    digitalWrite(CLK, HIGH);

    delayMicroseconds(100);

    digitalWrite(CLK, LOW);

    delayMicroseconds(100);

}

/* Moving the Linear Actuator may cause CW to be pulled high.
 * Therefore, it is recommend to set the preferred direction
 * after using LAC_move()
 */
void LAC_dir()
{
    delay(500);
    digitalWrite(CW, LOW);
}
