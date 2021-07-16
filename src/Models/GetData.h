#pragma once

struct GetData {
    unsigned int lightTimeSleepDurationInMinutes;
    unsigned int darkTimeSleepDurationInMinutes;
    unsigned int sendDataFrequency;
    unsigned int getDataFrequency;

    float safeModeVoltage;
    float economyModeVoltage;
    unsigned int economyModeDataSendSkipMultiplier;
    float solarVoltageForLightTime;

    const char* smsInformNumber;
};