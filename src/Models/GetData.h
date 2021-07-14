#pragma once

struct GetData {
    unsigned int lightTimeSleepDurationInMinutes;
    unsigned int darkTimeSleepDurationInMinutes;
    unsigned int sendDataFrequency;
    unsigned int getDataFrequency;

    float safeModeVoltage;
    float economyModeVoltage;

    String smsInformNumber;
};