#pragma once

struct GetData {
    unsigned int lightTimeSleepDurationSeconds;
    unsigned int darkTimeSleepDurationSeconds;
    unsigned int sendDataFrequency;

    float safeModeVoltage;
    float economyModeVoltage;
    unsigned int economyModeDataSendSkipMultiplier;
    float solarVoltageForLightTime;

    unsigned int version;
};