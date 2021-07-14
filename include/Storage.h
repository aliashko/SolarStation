#pragma once
#include <EEPROM.h>
#include <Models/Settings.h>

class Storage
{
public:
    Storage();
    Settings getSettings();
    void updateSettings(Settings settings);

    unsigned long getRestartsCount();
    void incrementRestartsCount();

private:
    const uint16_t _restartsCountOffset = 1000;
};