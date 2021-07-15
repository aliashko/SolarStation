#pragma once
#include <Models/Weather.h>
#include <Models/PowerLevels.h>
#include <Wire.h>
#include "SimpleDHT.h"
#include <INA219_WE.h>

class Sensors
{
public:
    Sensors();
    bool connect();
    
    Weather getWeather();
    PowerLevels getPowerLevels(bool useOnlyBuiltinSensors = false);

private:
    bool _isConnected;

    SimpleDHT22* _dht;
    INA219_WE* _ina219;

    float getVoltageFromAnalogPin(uint8_t pin, int r1, int r2);
};