#pragma once

struct GetData {
    int lightTimeSleepDurationInMinutes;
    int darkTimeSleepDurationInMinutes;
    int sendDataFrequency;
    int getDataFrequency;

    float safeModeVoltage;
    float economyModeVoltage;

    bool debugMode;
};