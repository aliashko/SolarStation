#pragma once

struct PostData {
    unsigned long timestamp;

    float temperature;
    float humidity;
    unsigned int raindropLevel;
    unsigned int soilMoistureLevel;

    uint8_t gsmSignalLevel;

    float solarVoltage;
    float solarCurrent;
    float batteryVoltage;
    float arduinoVoltage;
    float gsmVoltage;
    uint8_t powerMode;

    unsigned long restartsCount;
    unsigned int gsmErrors;
};