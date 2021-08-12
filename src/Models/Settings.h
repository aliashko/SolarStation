#pragma once
#include "Arduino.h"

struct Settings {
  unsigned int lightTimeSleepDurationInMinutes;
  unsigned int darkTimeSleepDurationInMinutes;
  unsigned int sendDataFrequency;
  unsigned int getDataFrequency;

  float safeModeVoltage;
  float economyModeVoltage;
  unsigned int economyModeDataSendSkipMultiplier;
  float solarVoltageForLightTime;

  unsigned long  smsInformNumber;

  unsigned long _integrityControlKey;
};

const unsigned long SETTINGS_INTEGRITY_CONTROL_KEY_VALUE = 123456789;

const Settings DEFAULT_SETTINGS = {
  .lightTimeSleepDurationInMinutes = 1,
  .darkTimeSleepDurationInMinutes = 1,
  .sendDataFrequency = 1,
  .getDataFrequency = 1,

  .safeModeVoltage = 3.5,
  .economyModeVoltage = 4.0,
  .economyModeDataSendSkipMultiplier = 10,
  .solarVoltageForLightTime = 4.0,

  .smsInformNumber = 0,

  ._integrityControlKey = 0
};
