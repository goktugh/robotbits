#include "utils.h"
#include "esp_system.h"
#include "esp_event.h"

void busy_sleep(uint64_t us)
{
    int64_t t0 = esp_timer_get_time();
    while (1) {
        int64_t t1 = esp_timer_get_time();
        if ((t1 - t0) > us) {
            break;
        }
    }
}
