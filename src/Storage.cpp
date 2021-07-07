#include "Storage.h"

Settings Storage::getSettings(){
    Settings settings;
    EEPROM.get(0, settings);

    if(settings._integrityControlKey != SETTINGS_INTEGRITY_CONTROL_KEY_VALUE){
        return DEFAULT_SETTINGS;
    }

    return settings;
}

void Storage::updateSettings(Settings settings){
    EEPROM.put(0, settings);
}