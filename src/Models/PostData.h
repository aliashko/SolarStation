#pragma once

struct PostData {
    unsigned long timestamp;

    float temperature;
    float humidity;
    unsigned int raindropLevel;
    unsigned int soilMoistureLevel;

    int gsmSignalLevel;

    float solarVoltage;
    float solarCurrent;
    float batteryVoltage;
    float arduinoVoltage;
    float gsmVoltage;
    uint8_t powerMode;

    int simMoneyBalance;

    unsigned long restartsCount;
    unsigned long gsmErrors;
};