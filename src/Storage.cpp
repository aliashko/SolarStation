#include "Storage.h"

//#define DEBUG

Storage::Storage(){}

Settings Storage::getDefaultSettings(){
    #ifdef DEBUG
    Serial.println(F("Storage::getDefaultSettings "));
    #endif
    return DEFAULT_SETTINGS;
}

Settings Storage::getSettings(){
    Settings settings;
    eeprom_read_block((void*)&settings, (void*)SETTINGS_OFFSET, sizeof(settings));

    #ifdef DEBUG
    Serial.print(F("Storage::getSettings "));
    Serial.print(F("darkTimeSleepDurationSeconds="));Serial.println(settings.darkTimeSleepDurationSeconds);
    Serial.print(F("lightTimeSleepDurationSeconds="));Serial.println(settings.lightTimeSleepDurationSeconds);
    Serial.print(F("sendDataFrequency="));Serial.println(settings.sendDataFrequency);
    Serial.print(F("SkipMultiplier="));Serial.println(settings.economyModeDataSendSkipMultiplier);
    Serial.print(F("resetSend="));Serial.println(settings.resetSendDataCounterAfterFailure);
    Serial.print(F("econV="));Serial.println(settings.economyModeVoltage);
    Serial.print(F("safeV="));Serial.println(settings.safeModeVoltage);
    Serial.print(F("solarV="));Serial.println(settings.solarVoltageForLightTime);
    Serial.print(F("_version="));Serial.println(settings._version);
    Serial.print(F("_integrityControlKey="));Serial.println(settings._integrityControlKey);
    #endif

    if(settings._integrityControlKey != SETTINGS_INTEGRITY_CONTROL_KEY_VALUE || !isSettingsValid(settings)){
        #ifdef DEBUG
        Serial.println(F("Default settings returned"));
        #endif
        return DEFAULT_SETTINGS;
    }

    return settings;
}

void Storage::updateSettings(Settings settings){
    #ifdef DEBUG
    Serial.print(F("Storage::updateSettings"));
    Serial.print(F("darkTimeSleepDurationSeconds="));Serial.println(settings.darkTimeSleepDurationSeconds);
    Serial.print(F("lightTimeSleepDurationSeconds="));Serial.println(settings.lightTimeSleepDurationSeconds);
    Serial.print(F("sendDataFrequency="));Serial.println(settings.sendDataFrequency);
    Serial.print(F("SkipMultiplier="));Serial.println(settings.economyModeDataSendSkipMultiplier);
    Serial.print(F("resetSend="));Serial.println(settings.resetSendDataCounterAfterFailure);
    Serial.print(F("econV="));Serial.println(settings.economyModeVoltage);
    Serial.print(F("safeV="));Serial.println(settings.safeModeVoltage);
    Serial.print(F("solarV="));Serial.println(settings.solarVoltageForLightTime);
    Serial.print(F("_version="));Serial.println(settings._version);
    Serial.print(F("_integrityControlKey="));Serial.println(settings._integrityControlKey);
    #endif

    if(!isSettingsValid(settings))return;        

    eeprom_update_block((void*)&settings, (void*)SETTINGS_OFFSET, sizeof(settings));
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