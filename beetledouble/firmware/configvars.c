#include "configvars.h"

#include <stdlib.h>

#include "diag.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>

// Current config:
config_values_t config_current;

// Structure holding the variable definitions
const config_var_t all_config_vars[] = {    
    // Name, default, min, max
    {"Mixing (0=off 1=on, 2=depends on JP1)", 2, 0, 2, &(config_current.mixing_on) },
    {"Mixing amount (/16)", 11, 4, 16, &(config_current.mixing_amount)},
    {"Braking on", 1, 0, 1, &(config_current.braking_on)},
    {"Low Voltage cutoff 3S x0.1V", 96, 0, 120, &(config_current.voltage_cutoff_3s)},
    {"Low Voltage cutoff 4S x0.1V", 128, 0, 160, &(config_current.voltage_cutoff_4s)},
    {"Overcurrent limit (instantaneous) Amps", 25, 0, 35, &(config_current.overcurrent_limit)},
    {"Startup sound on",  1, 0, 1, &(config_current.startup_sound_on)},
    {"Startup sound delay (x0.1 seconds)",  0, 0, 50, &(config_current.startup_sound_delay)},
    {NULL, 0, 0, 0, NULL}
};

#define MAGIC_NUMBER 0x2a91

// Eeprom storage:
// Offset 0 = magic number
// offset 2 = (uint8_t) sizeof(config_values_t)
// offset 3 = config_values_t

// If the magic number is incorrect or the size of the structure is
// different, we load the defaults instead.

void configvars_load_defaults()
{
    uint8_t i=0;
    while (all_config_vars[i].var_ptr != NULL)
    {
        *(all_config_vars[i].var_ptr) =
            all_config_vars[i].default_value;
        ++ i;
    }
}

static void * magic_addr_eeprom = (void *) 0;
static void * size_addr_eeprom = (void *) 2;
static void * vars_addr_eeprom = (void *) 3;

void configvars_load()
{
    uint16_t magic_read;
    // Read the 2 byte magic number
    eeprom_read_block(&magic_read, magic_addr_eeprom, 2);
    uint8_t size_read;
    eeprom_read_block(&size_read, size_addr_eeprom, 1);
    diag_println("configvars: magic %02x size %d",
        magic_read, size_read);
    if ((magic_read == MAGIC_NUMBER) && 
        size_read == sizeof(config_values_t)) {
        diag_println("configvars: Loading config from eeprom");
        eeprom_read_block(&config_current, vars_addr_eeprom, sizeof(config_values_t));
    } else {
        diag_println("configvars: Loading config from defaults");
        configvars_load_defaults();
    }
}

void configvars_save()
{
    // Write the magic number and length
    uint16_t magic = MAGIC_NUMBER;
    uint8_t len = (uint8_t) sizeof(config_values_t);
    eeprom_update_block(&magic, magic_addr_eeprom, 2);
    eeprom_update_block(&len, size_addr_eeprom, 1);
    eeprom_update_block(&config_current, vars_addr_eeprom, sizeof(config_values_t));
}

uint8_t configvars_count()
{
    uint8_t i;
    for (i=0; all_config_vars[i].var_ptr != NULL; i++) ;
    return i;
}
