#pragma once
#include "PowerManager.h"
#include "Storage.h"
#include "Sensors.h"
#include "WebClient.h"
#include "Models/SystemState.h"

class LifecycleManager
{
public:
    LifecycleManager();
    void iterate();

    void initialize();
    void measureForSafeMode();
    void measure();
    void sendData();
    void getUpdates();

private:
    Settings _settings;
    SystemState _systemState;

    PowerManager* _powerManager;
    Storage* _storage;
    Sensors* _sensors;
    WebClient* _webClient;

    unsigned int _sendDataIterationCounter = 0;
    unsigned int _getDataIterationCounter = 0;
    Weather _currentWeather;
    PowerLevels _currentPowerLevels;

    void updateSystemState();
    void sleep();
};