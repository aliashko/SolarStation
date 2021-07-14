#pragma once

struct PostData {
    unsigned long timestamp;

    float temperature;
    float humidity;
    unsigned int raindropLevel;

    float solarVoltage;
    float solarCurrent;
    float batteryVoltage;
    float arduinoVoltage;
    float gsmVoltage;

    bool isSafeMode;
    bool isEconomyMode;
    unsigned long restartsCount;
    unsigned int gsmErrors;
};