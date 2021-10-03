#pragma once

struct GetData {
    unsigned int lightTimeSleepDurationSeconds;
    unsigned int darkTimeSleepDurationSeconds;
    unsigned int sendDataFrequency;
    unsigned int sendSupplementalDataFrequency;
    bool resetSendDataCounterAfterFailure;

    float safeModeVoltage;
    float economyModeVoltage;
    unsigned int economyModeDataSendSkipMultiplier;
    float solarVoltageForLightTime;

    unsigned int version;
};