#include "controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ps3.h"
#include "esp_bt_device.h"

#include <string.h>

controller_state_t controller_state; 

#define SPIN_THROTTLE_INCREMENT 5
#define SPIN_THROTTLE_MAX 100

static void clear_state()
{
    memset(&controller_state, 0, sizeof(controller_state));
}

static void clamp(int *var, int minval, int maxval)
{
    if (*var > maxval) 
        *var = maxval;
    if (*var < minval)
        *var = minval;
}

static void clamp_state()
{
    clamp(& controller_state.spin_throttle, 0, SPIN_THROTTLE_MAX);
    clamp(& controller_state.drive_throttle, -100, 100);
    clamp(& controller_state.steering, -100, 100);
}

static void controller_event_cb( ps3_t ps3, ps3_event_t event )
{
    // Event handling here...
    if (event.button_down.cross) {
        printf("PS3: cross button down\n");
    }
    if (event.button_down.up) {
        // Increase spin speed.
        controller_state.spin_throttle += SPIN_THROTTLE_INCREMENT;         
    }
    if (event.button_down.down) {
        // Decrease spin speed.
        controller_state.spin_throttle -= SPIN_THROTTLE_INCREMENT;         
    }
    // Stop buttons
    if (event.button_down.start || event.button_down.select) {
        controller_state.spin_throttle = 0;
    }
    // Led test
    if (event.button_down.triangle) {
        controller_state.led_test = 1;
    }
    if (event.button_up.triangle) {
        controller_state.led_test = 0;
    }
    
    // Drive / steering on right hand stick
    // These are signed 8-bit.
    controller_state.drive_throttle = event.analog_changed.stick.ry;
    controller_state.steering = event.analog_changed.stick.rx;
    
    
    clamp_state();
}

static void controller_connected_cb( uint8_t is_connected )
{
    if (is_connected) {
        printf("PS3 controller connected ok\n");
        ps3SetLed(1);
        controller_state.got_signal = 1;
    } else {
        printf("PS3 controller disconnected\n");
        clear_state(); // Failsafe - also clears got_signal
    }
}

void controller_init()
{
    ps3SetEventCallback(controller_event_cb);
    ps3SetConnectionCallback( controller_connected_cb );

    ps3Init();
    
    printf("Local bt address:");
    const uint8_t * myaddr = esp_bt_dev_get_address();
    for (int i=0; i<6; i++) {
        if (i>0) putchar(':');
        printf("%02x", myaddr[i]);
    }
    printf("\n");
}
