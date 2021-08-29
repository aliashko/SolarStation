#include "Storage.h"

//#define DEBUG

Storage::Storage(){}

Settings Storage::getSettings(){
    Settings settings;
    eeprom_read_block((void*)&settings, (void*)100, sizeof(settings));

    #ifdef DEBUG
    Serial.print(F("Storage::getSettings "));
    Serial.print(F("lightTimeSleepDurationInMinutes="));Serial.print(settings.lightTimeSleepDurationSeconds);
    Serial.print(F(" econ="));Serial.print(settings.economyModeVoltage);
    Serial.print(F(" _integrityControlKey="));Serial.println(settings._integrityControlKey);
    #endif

    if(settings._integrityControlKey != SETTINGS_INTEGRITY_CONTROL_KEY_VALUE){
        return DEFAULT_SETTINGS;
    }

    return settings;
}

void Storage::updateSettings(Settings settings){
    #ifdef DEBUG
    Serial.print(F("Storage::updateSettings sizeof="));Serial.println((int)sizeof(settings));
    Serial.print(F("Storage::updateSettings econ="));Serial.println(settings.economyModeVoltage);
    #endif
    eeprom_update_block((void*)&settings, (void*)100, sizeof(settings));
    #ifdef DEBUG
    Serial.println(F("Storage::updateSettings"));
    #endif
}

unsigned long Storage::getRestartsCount(){
    unsigned long count;
    eeprom_read_block((void*)&count, (const void*)RESTARTS_COUNT_OFFSET, sizeof(count));

    #ifdef DEBUG
    Serial.print(F("Storage::getRestartsCount = "));Serial.println(count);
    #endif

    return count;
}

void Storage::updateRestartsCount(unsigned long updatedCount){
    eeprom_update_block((void*)&updatedCount, (void*)RESTARTS_COUNT_OFFSET, sizeof(updatedCount));
    
    #ifdef DEBUG
    Serial.print(F("Storage::updateRestartsCount = "));Serial.println(updatedCount);
    #endif
}