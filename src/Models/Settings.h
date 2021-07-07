struct Settings {
  int lightTimeSleepDurationInMinutes;
  int darkTimeSleepDurationInMinutes;
  int sendDataFrequency;
  int getDataFrequency;

  float safeModeVoltage;
  float economyModeVoltage;

  bool debugMode;
  int _integrityControlKey;
};

const int SETTINGS_INTEGRITY_CONTROL_KEY_VALUE = (int)12345678;

Settings DEFAULT_SETTINGS = Settings {
  .lightTimeSleepDurationInMinutes = 1,
  .darkTimeSleepDurationInMinutes = 1,
  .sendDataFrequency = 1,
  .getDataFrequency = 1,

  .safeModeVoltage = 3.5,
  .economyModeVoltage = 4.0,

  .debugMode = true,
  ._integrityControlKey = SETTINGS_INTEGRITY_CONTROL_KEY_VALUE
};