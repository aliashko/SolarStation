#include "LifecycleManager.h"
#include "DebugModeManager.h"
#include "config.h"

LifecycleManager::LifecycleManager() {
    _powerManager = new PowerManager();
    _storage = new Storage();
    _sensors = new Sensors();
    _webClient = new WebClient();

    initialize();
}

void LifecycleManager::iterate() {
    _sendDataIterationCounter++;
    _getDataIterationCounter++;  

    if(_systemState.powerMode == PowerMode::Safe){
       measureForSafeMode();
    }
    else {
       measure();
    }

    updateSystemState();

    if(_systemState.powerMode != PowerMode::Safe){
        if(_sendDataIterationCounter >= 
            _settings.sendDataFrequency * (_systemState.powerMode == PowerMode::Economy?_settings.economyModeDataSendSkipMultiplier:1)){
            sendData();
        }

        if(_systemState.powerMode == PowerMode::Powerfull){
            if(_getDataIterationCounter >= _settings.getDataFrequency){
                getUpdates();
            }
        }
    }

    sleep();
}

void LifecycleManager::initialize(){
    DebugModeManager::initializeHardware();

    _settings = _storage->getSettings();
    _systemState = SystemState{
        .timestamp = 0,
        .isDebugMode = DebugModeManager::checkIfDebugModeRequested(),
        .powerMode = PowerMode::Unknown,
        .restartsCount = _storage->getRestartsCount(),
        .gsmErrors = 0,
    };

    if(_systemState.isDebugMode)DebugModeManager::blinkAllLeds();
    else _storage->incrementRestartsCount();
}

void LifecycleManager::measure(){
    if(_systemState.isDebugMode) DebugModeManager::greenLedMode(true);
    _powerManager->changeSensorsPower(true);
    delay(SENSORS_WARMUP_DELAY_MS);

    _sensors->connect();
    _currentWeather = _sensors->getWeather();
    _currentPowerLevels = _sensors->getPowerLevels();

    _powerManager->changeSensorsPower(false);
    if(_systemState.isDebugMode) DebugModeManager::greenLedMode(false);
}

void LifecycleManager::measureForSafeMode(){
    _currentPowerLevels = _sensors->getPowerLevels();
}

void LifecycleManager::sendData(){
    if(_systemState.isDebugMode){ DebugModeManager::greenLedMode(true);DebugModeManager::blueLedMode(true);}
    _powerManager->changeGsmPower(true);
    delay(GSM_WARMUP_DELAY_MS);

    PostData data = {
        .timestamp = _systemState.timestamp,

        .temperature = _currentWeather.temperature,
        .humidity = _currentWeather.humidity,
        .raindropLevel = _currentWeather.raindropLevel,

        .solarVoltage = _currentPowerLevels.solarVoltage,
        .solarCurrent = _currentPowerLevels.solarCurrent,
        .batteryVoltage = _currentPowerLevels.batteryVoltage,
        .arduinoVoltage = _currentPowerLevels.arduinoVoltage,
        .gsmVoltage = _currentPowerLevels.gsmVoltage,

        .powerMode = (uint8_t)_systemState.powerMode,
        .restartsCount = _systemState.restartsCount,
        .gsmErrors = _systemState.gsmErrors
    };
    if(!_webClient->postData(data))_systemState.gsmErrors++;

    _powerManager->changeGsmPower(false);
    if(_systemState.isDebugMode){ DebugModeManager::greenLedMode(false);DebugModeManager::blueLedMode(false);}
}

void LifecycleManager::getUpdates(){
    if(_systemState.isDebugMode) DebugModeManager::blueLedMode(true);
    _powerManager->changeGsmPower(true);
    delay(GSM_WARMUP_DELAY_MS);

    GetData data;
    if(_webClient->getUpdates(&data)){
        _settings.lightTimeSleepDurationInMinutes = data.lightTimeSleepDurationInMinutes;
        _settings.darkTimeSleepDurationInMinutes = data.darkTimeSleepDurationInMinutes;
        _settings.sendDataFrequency = data.sendDataFrequency;
        _settings.getDataFrequency = data.getDataFrequency;
        _settings.safeModeVoltage = data.safeModeVoltage;
        _settings.economyModeVoltage = data.economyModeVoltage;
        _settings.smsInformNumber = data.smsInformNumber;
    }
    else _systemState.gsmErrors++;

    _powerManager->changeGsmPower(false);
    if(_systemState.isDebugMode)DebugModeManager::blueLedMode(false);
}

void LifecycleManager::updateSystemState(){    
    _systemState.timestamp = millis() / 1000;

    if(_currentPowerLevels.batteryVoltage == 0)_systemState.powerMode = PowerMode::Unknown;
    else if(_currentPowerLevels.batteryVoltage <= _settings.safeModeVoltage)_systemState.powerMode = PowerMode::Safe;
    else if(_currentPowerLevels.batteryVoltage <= _settings.economyModeVoltage)_systemState.powerMode = PowerMode::Economy;
    else _systemState.powerMode = PowerMode::Powerfull;

    if(_systemState.isDebugMode){
        DebugModeManager::redLedMode(false);DebugModeManager::yellowLedMode(false);
        if(_systemState.powerMode == PowerMode::Safe)DebugModeManager::redLedMode(true);
        else if(_systemState.powerMode == PowerMode::Economy)DebugModeManager::yellowLedMode(true);
        else if(_systemState.powerMode == PowerMode::Unknown){DebugModeManager::yellowLedMode(true);DebugModeManager::redLedMode(true);}
    }
}

void LifecycleManager::sleep(){    
    if(_currentPowerLevels.solarVoltage >= _settings.solarVoltageForLightTime)
        _powerManager->deepSleep(_settings.lightTimeSleepDurationInMinutes * 60);

    _powerManager->deepSleep(_settings.darkTimeSleepDurationInMinutes * 60);
}