
#include <stdint.h>

typedef struct {
    const char *name;
    uint8_t default_value;
    uint8_t min_value;
    uint8_t max_value;
    uint8_t *var_ptr;
} config_var_t;

/*
 * This single instance structure holds the current config.
 */
typedef struct {
    uint8_t mixing_on;
    uint8_t mixing_amount;
    uint8_t braking_on;
    uint8_t voltage_cutoff_3s;
    uint8_t voltage_cutoff_4s;
    uint8_t overcurrent_limit;
    uint8_t startup_sound_on;
} config_values_t;

// Current config - held in ram.
extern config_values_t config_current;

extern const config_var_t all_config_vars[];

// Load from eeprom - called at init.
void configvars_load();

// Load default values from flash
void configvars_load_defaults();
// Save to eeprom - called when variable changed in config.
void configvars_save();

// Return the number of config vars
uint8_t configvars_count();
