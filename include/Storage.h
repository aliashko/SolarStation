#pragma once
#include <avr/eeprom.h>
#include <Models/Settings.h>

#define RESTARTS_COUNT_OFFSET 1000

class Storage
{
public:
    Storage();
    Settings getSettings();
    void updateSettings(Settings settings);

    unsigned long getRestartsCount(unsigned long integrityControlKey = SETTINGS_INTEGRITY_CONTROL_KEY_VALUE);
    void updateRestartsCount(unsigned long updatedCount);
};