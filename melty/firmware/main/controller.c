#include "controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ps3.h"
#include "esp_bt_device.h"

static void controller_event_cb( ps3_t ps3, ps3_event_t event )
{
    // Event handling here...
    if (event.button_down.cross) {
        printf("PS3: cross button down\n");
    }
}

static void controller_connected_cb( uint8_t is_connected )
{
    if (is_connected) {
        printf("PS3 controller connected ok\n");
        ps3SetLed(1);
    } else {
        printf("PS3 controller disconnected\n");        
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
