#include <EEPROM.h>
#include <Models/Settings.h>
#pragma once

class Storage
{
public:
    Storage();
    Settings getSettings();
    void updateSettings(Settings settings);
};