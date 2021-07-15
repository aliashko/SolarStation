#pragma once
#include <Models/Weather.h>
#include <Models/PowerLevels.h>
#include <Wire.h>
#include "DHT.h"
#include <Adafruit_INA219.h>

class Sensors
{
public:
    Sensors();
    bool connect();
    
    Weather getWeather();
    PowerLevels getPowerLevels(bool useOnlyBuiltinSensors = false);

private:
    bool _isConnected;

    DHT* _dht;
    Adafruit_INA219* _ina219;

    float getVoltageFromAnalogPin(uint8_t pin, int r1, int r2);
};