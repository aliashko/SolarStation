#pragma once
#include <avr/eeprom.h>
#include <Models/Settings.h>

#define SETTINGS_OFFSET 0
#define RESTARTS_COUNT_OFFSET 1000

class Storage
{
public:
    Storage();
    Settings getSettings();
    Settings getDefaultSettings();
    void updateSettings(Settings settings);

    unsigned long getRestartsCount();
    void updateRestartsCount(unsigned long updatedCount);
};