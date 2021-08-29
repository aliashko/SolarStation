#pragma once
#include "Arduino.h"

struct Settings {
  unsigned int lightTimeSleepDurationSeconds;
  unsigned int darkTimeSleepDurationSeconds;
  unsigned int sendDataFrequency;

  float safeModeVoltage;
  float economyModeVoltage;
  unsigned int economyModeDataSendSkipMultiplier;
  float solarVoltageForLightTime;

  unsigned int _version;
  unsigned long _integrityControlKey;
};

const unsigned long SETTINGS_INTEGRITY_CONTROL_KEY_VALUE = 123456799;

const Settings DEFAULT_SETTINGS = {
  .lightTimeSleepDurationSeconds = 3 * 60,
  .darkTimeSleepDurationSeconds = 20 * 60,
  .sendDataFrequency = 1,

  .safeModeVoltage = 3.7,
  .economyModeVoltage = 4.5,
  .economyModeDataSendSkipMultiplier = 9,
  .solarVoltageForLightTime = 3.0,

  ._version = 1,
  ._integrityControlKey = 0
};
