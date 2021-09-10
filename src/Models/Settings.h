#pragma once
#include "Arduino.h"

struct Settings {
  unsigned int lightTimeSleepDurationSeconds;
  unsigned int darkTimeSleepDurationSeconds;
  unsigned int sendDataFrequency;  
  bool resetSendDataCounterAfterFailure;

  float safeModeVoltage;
  float economyModeVoltage;
  unsigned int economyModeDataSendSkipMultiplier;
  float solarVoltageForLightTime;

  unsigned int _version;
  unsigned long _integrityControlKey;
};

const unsigned long SETTINGS_INTEGRITY_CONTROL_KEY_VALUE = 123456789;

const Settings DEFAULT_SETTINGS = {
  .lightTimeSleepDurationSeconds = 3 * 60,
  .darkTimeSleepDurationSeconds = 20 * 60,
  .sendDataFrequency = 1,
  .resetSendDataCounterAfterFailure = false,

  .safeModeVoltage = 4.2,
  .economyModeVoltage = 4.7,
  .economyModeDataSendSkipMultiplier = 9,
  .solarVoltageForLightTime = 3.0,

  ._version = 1,
  ._integrityControlKey = 0
};

static bool isSettingsValid(Settings settings){
    if(settings.darkTimeSleepDurationSeconds > 11000
        || settings.lightTimeSleepDurationSeconds > 11000
        || settings.sendDataFrequency > 1000
        || settings.economyModeDataSendSkipMultiplier > 1000
        || settings.economyModeVoltage > 6.0
        || settings.safeModeVoltage > 6.0
        || settings.solarVoltageForLightTime > 6.0) {
            return false;
        }
    return true;
}