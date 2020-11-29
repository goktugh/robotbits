#include "controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ps3.h"

static void controller_event_cb( ps3_t ps3, ps3_event_t event )
{
    // Event handling here...
}

void controller_init()
{
    ps3SetEventCallback(controller_event_cb);
    ps3Init();
}
