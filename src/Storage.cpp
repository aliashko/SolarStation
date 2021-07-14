#include "Storage.h"

Storage::Storage(){}

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

unsigned long Storage::getRestartsCount(){
    Settings settings;
    EEPROM.get(0, settings);

    if(settings._integrityControlKey != SETTINGS_INTEGRITY_CONTROL_KEY_VALUE){
        return 0;
    }

    unsigned long count;
    EEPROM.get(_restartsCountOffset, count);
    return count;
}

void Storage::incrementRestartsCount(){
    auto count = getRestartsCount();
    count++;
    EEPROM.put(_restartsCountOffset, count);
}