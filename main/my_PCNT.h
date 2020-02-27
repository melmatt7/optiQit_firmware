/* SECTION: Summary
 *  
 * Use PCNT module to count rising edges generated externally or internally by ESP32 LEDC module.
 *
 * Functionality of relevant GPIOs:
 *   - GPIO18 - output pin of internal 1 Hz pulse generator.
 *   - GPIO4 - pulse input pin,
 *   - GPIO5 - control input pin.
 *
 * Upload example, open the serial monitor to view the message printed on your screen.
 * You may need to unplug and replug the USB after uploading to view in the serial monitor
 *
 * To do this test, you should connect GPIO18 with GPIO4 or connect an external pulse generator to GPIO4.
 * GPIO5 is the control signal, you can leave it floating with internal pull up,
 * or connect it to ground. If left floating, the count value will be increasing.
 * If you connect GPIO5 to GND, the count value will be decreasing.
 *
 * An interrupt will be triggered when the counter value:
 *   - reaches 'thresh1' or 'thresh0' value,
 *   - reaches 'l_lim' value or 'h_lim' value,
 *   - will be reset to zero.
 */

/* SECTION: Pulse Counter Libraries & Definitions */
#include "driver/pcnt.h"

/* Pulse Counter Channel 0 definitions */
#define PCNT_TEST_UNIT_0       PCNT_UNIT_0
#define PCNT_H_LIM_VAL_0       INT16_MAX   // Max count value
#define PCNT_L_LIM_VAL_0       -10
#define PCNT_THRESH1_VAL_0     INT16_MAX   // Trigger an interrupt after this many counts
#define PCNT_THRESH0_VAL_0    -10
#define PCNT_INPUT_SIG_IO_0    16           // Pulse Input 0 GPIO
#define PCNT_INPUT_CTRL_IO_0   17           // Control GPIO 0 HIGH=count up, LOW=count down
/* Pulse Counter Channel 0 definitions */
#define PCNT_TEST_UNIT_1       PCNT_UNIT_1
#define PCNT_H_LIM_VAL_1       INT16_MAX   // Max count value
#define PCNT_L_LIM_VAL_1      -10
#define PCNT_THRESH1_VAL_1     INT16_MAX   // Trigger an interrupt after this many counts
#define PCNT_THRESH0_VAL_1    -10
#define PCNT_INPUT_SIG_IO_1    18           // Pulse Input 1 GPIO
#define PCNT_INPUT_CTRL_IO_1   19           // Control GPIO 1 HIGH=count up, LOW=count down

typedef struct {                            // Sample structure to pass events from PCNT interrupt handler to main program
    int unit;                               // the PCNT unit that originated an interrupt
    uint32_t status;                        // information on the event type that caused the interrupt
} pcnt_evt_t;

/* SECTION: pcnt_init function */
/* Initialize PCNT functions:
 *  - configure and initialize PCNT
 *  - set up the input filter
 *  - set up the counter events to watch
 */
static esp_err_t pcnt_init_and_start(void)
{
    esp_err_t success;
    /* Prepare configuration for the PCNT unit 0 */
    pcnt_config_t pcnt_config_0;
    // Set PCNT input signal and control GPIOs
    pcnt_config_0.pulse_gpio_num = PCNT_INPUT_SIG_IO_0;
    pcnt_config_0.ctrl_gpio_num  = PCNT_INPUT_CTRL_IO_0;
    pcnt_config_0.channel        = PCNT_CHANNEL_0;
    pcnt_config_0.unit           = PCNT_TEST_UNIT_0;
    // What to do on the positive / negative edge of pulse input?
    pcnt_config_0.pos_mode       = PCNT_COUNT_INC;      // Count up on the positive edge
    pcnt_config_0.neg_mode       = PCNT_COUNT_DIS;      // Keep the counter value on the negative edge
    // What to do when control input is low or high?
    pcnt_config_0.lctrl_mode     = PCNT_MODE_REVERSE;   // Reverse counting direction if low
    pcnt_config_0.hctrl_mode     = PCNT_MODE_KEEP;      // Keep the primary counter mode if high
    // Set the maximum and minimum limit values to watch
    pcnt_config_0.counter_h_lim  = PCNT_H_LIM_VAL_0;
    pcnt_config_0.counter_l_lim  = PCNT_L_LIM_VAL_0;

    /* Prepare configuration for the PCNT unit 1 */
    pcnt_config_t pcnt_config_1;
    // Set PCNT input signal and control GPIOs
    pcnt_config_1.pulse_gpio_num = PCNT_INPUT_SIG_IO_1;
    pcnt_config_1.ctrl_gpio_num  = PCNT_INPUT_CTRL_IO_1;
    pcnt_config_1.channel        = PCNT_CHANNEL_1;
    pcnt_config_1.unit           = PCNT_TEST_UNIT_1;
    // What to do on the positive / negative edge of pulse input?
    pcnt_config_1.pos_mode       = PCNT_COUNT_INC;      // Count up on the positive edge
    pcnt_config_1.neg_mode       = PCNT_COUNT_DIS;      // Keep the counter value on the negative edge
    // What to do when control input is low or high?
    pcnt_config_1.lctrl_mode     = PCNT_MODE_REVERSE;   // Reverse counting direction if low
    pcnt_config_1.hctrl_mode     = PCNT_MODE_KEEP;      // Keep the primary counter mode if high
    // Set the maximum and minimum limit values to watch
    pcnt_config_1.counter_h_lim  = PCNT_H_LIM_VAL_1;
    pcnt_config_1.counter_l_lim  = PCNT_L_LIM_VAL_1;

    /* Initialize PCNT units */
    success = pcnt_unit_config(&pcnt_config_0);
    success = pcnt_unit_config(&pcnt_config_1);

    if (success != ESP_OK) {
      printf("FAILED: my-pcnt.h >> pcnt_unit_config failed");
      return success;
    }

    /* [Optional] Configure and enable the input filter (to filter glitches */
    //success = pcnt_set_filter_value(PCNT_TEST_UNIT_0, 100);
    //success = pcnt_set_filter_value(PCNT_TEST_UNIT_1, 100);
    //success = pcnt_filter_enable(PCNT_TEST_UNIT_0);
    //success = pcnt_filter_enable(PCNT_TEST_UNIT_1);

    /* [Optional] Set threshold 0 and 1 values and enable events to watch */
    //success = pcnt_set_event_value(PCNT_TEST_UNIT_0, PCNT_EVT_THRES_1, PCNT_THRESH1_VAL_0);
    //success = pcnt_event_enable(PCNT_TEST_UNIT_0, PCNT_EVT_THRES_1);
    //success = pcnt_set_event_value(PCNT_TEST_UNIT_0, PCNT_EVT_THRES_0, PCNT_THRESH0_VAL_0);
    //success = pcnt_event_enable(PCNT_TEST_UNIT_0, PCNT_EVT_THRES_0);
    //success = pcnt_set_event_value(PCNT_TEST_UNIT_1, PCNT_EVT_THRES_1, PCNT_THRESH1_VAL_1);
    //success = pcnt_event_enable(PCNT_TEST_UNIT_1, PCNT_EVT_THRES_1);
    //success = pcnt_set_event_value(PCNT_TEST_UNIT_1, PCNT_EVT_THRES_0, PCNT_THRESH0_VAL_1);
    //success = pcnt_event_enable(PCNT_TEST_UNIT_1, PCNT_EVT_THRES_0);
    /* [Optional] Enable events on zero, maximum and minimum limit values */
    //success = pcnt_event_enable(PCNT_TEST_UNIT_0, PCNT_EVT_ZERO);
    //success = pcnt_event_enable(PCNT_TEST_UNIT_0, PCNT_EVT_H_LIM_0);
    //success = pcnt_event_enable(PCNT_TEST_UNIT_0, PCNT_EVT_L_LIM_0);
    //success = pcnt_event_enable(PCNT_TEST_UNIT_1, PCNT_EVT_ZERO);
    //success = pcnt_event_enable(PCNT_TEST_UNIT_1, PCNT_EVT_H_LIM_1);
    //success = pcnt_event_enable(PCNT_TEST_UNIT_1, PCNT_EVT_L_LIM_1);

    /* Initialize PCNT's counter */
    success = pcnt_counter_pause(PCNT_TEST_UNIT_0);
    success = pcnt_counter_pause(PCNT_TEST_UNIT_1);
    success = pcnt_counter_clear(PCNT_TEST_UNIT_0);
    success = pcnt_counter_clear(PCNT_TEST_UNIT_1);

    if (success != ESP_OK) {
      printf("FAILED: my-pcnt.h >> pcnt_counter_pause or pcnt_counter_clear failed");
      return success;
    }

    /* [Optional] Register ISR handler and enable interrupts for PCNT unit */
    //pcnt_isr_register(pcnt_example_intr_handler, NULL, 0, &user_isr_handle);
    //pcnt_intr_enable(PCNT_TEST_UNIT_0);
    //pcnt_intr_enable(PCNT_TEST_UNIT_1);

    /* Everything is set up, now go to counting */
    success = pcnt_counter_resume(PCNT_TEST_UNIT_0);
    success = pcnt_counter_resume(PCNT_TEST_UNIT_1);

    return success;
}

static esp_err_t pcnt_clear(void) {
    esp_err_t success;
    
    success = pcnt_counter_pause(PCNT_TEST_UNIT_0);
    success = pcnt_counter_clear(PCNT_TEST_UNIT_0);

    return success;
}

static esp_err_t pcnt_get(int16_t* pcount_0, int16_t* pcount_1) {
    esp_err_t success;
    
    success = pcnt_counter_pause(PCNT_TEST_UNIT_0);
    success = pcnt_counter_pause(PCNT_TEST_UNIT_1);
    success = pcnt_get_counter_value(PCNT_TEST_UNIT_0, pcount_0);
    success = pcnt_get_counter_value(PCNT_TEST_UNIT_1, pcount_1);
    success = pcnt_counter_resume(PCNT_TEST_UNIT_0);
    success = pcnt_counter_resume(PCNT_TEST_UNIT_1);
    
    return success;
}

static esp_err_t pcnt_get(int16_t* pcount_0) {
    esp_err_t success;
    
    success = pcnt_counter_pause(PCNT_TEST_UNIT_0);
    success = pcnt_get_counter_value(PCNT_TEST_UNIT_0, pcount_0);
    success = pcnt_counter_resume(PCNT_TEST_UNIT_0);
    
    return success;
}
