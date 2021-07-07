#include <EEPROM.h>
#include <Models/Settings.h>

class Storage
{
public:
    Storage();
    Settings getSettings();
    void updateSettings(Settings settings);
};