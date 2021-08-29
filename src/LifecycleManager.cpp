#include "LifecycleManager.h"
#include "DebugModeManager.h"
#include "config.h"
#include "GyverWDT.h"

extern void safeDelay(unsigned int ms);
//#define DEBUG

LifecycleManager::LifecycleManager() {
    _powerManager = new PowerManager();
    _storage = new Storage();
    _sensors = new Sensors();
    _webClient = new WebClient();

    initialize();
}

void LifecycleManager::initialize(){
    #ifdef DEBUG
    Serial.println("LifecycleManager::initialize");
    #endif
    DebugModeManager::initializeHardware();

    _settings = _storage->getSettings();
    _systemState = SystemState{
        .timestamp = 0,
        .isDebugMode = DebugModeManager::checkIfDebugModeRequested(),
        .powerMode = SystemPowerMode::Safe
    };
    _systemState.gsmErrors = 0;

    if(_settings._integrityControlKey == SETTINGS_INTEGRITY_CONTROL_KEY_VALUE){
        _systemState.restartsCount = _storage->getRestartsCount();
    }
    else {
        #ifdef DEBUG
        Serial.print(">>> settings INTEGRITY mismatch ");Serial.println(_settings._integrityControlKey);
        #endif
        _systemState.restartsCount = 0;
        _settings._integrityControlKey = SETTINGS_INTEGRITY_CONTROL_KEY_VALUE;
        _storage->updateSettings(_settings);
    }

    if(_systemState.isDebugMode)DebugModeManager::blinkAllLeds();
    else _storage->updateRestartsCount(_systemState.restartsCount + 1);
}

void LifecycleManager::iterate() {
    #ifdef DEBUG
    Serial.println("LifecycleManager::iterate");
    #endif
    _sendDataIterationCounter++;

    if(_systemState.powerMode == SystemPowerMode::Safe){
       measureForSafeMode();
       updateSystemState();
    }
    else {
        measure();
        updateSystemState();

        if(_systemState.powerMode != SystemPowerMode::Safe){
            if(_sendDataIterationCounter >= 
                _settings.sendDataFrequency * (_systemState.powerMode == SystemPowerMode::Economy?_settings.economyModeDataSendSkipMultiplier:1)){
                sendData();
            }
        }
    }

    Watchdog.reset();
    sleep();
}

void LifecycleManager::measure(){
    #ifdef DEBUG
    Serial.println("LifecycleManager::measure");
    #endif
    if(_systemState.isDebugMode) DebugModeManager::greenLedMode(true);
    _powerManager->changeSensorsPower(true);
    safeDelay(SENSORS_WARMUP_DELAY_MS);
    Watchdog.reset();

    _sensors->connect();

    _currentWeather = _sensors->getWeather();
    _currentPowerLevels = _sensors->getPowerLevels();
  
    _powerManager->changeSensorsPower(false);
    if(_systemState.isDebugMode) DebugModeManager::greenLedMode(false);
}

void LifecycleManager::measureForSafeMode(){
    #ifdef DEBUG
    Serial.println("LifecycleManager::measureForSafeMode");
    #endif

    _currentPowerLevels = _sensors->getPowerLevels(true);
}

void LifecycleManager::sendData(){
    #ifdef DEBUG
    Serial.println("LifecycleManager::sendData");
    #endif

    if(_systemState.isDebugMode){ DebugModeManager::greenLedMode(true);DebugModeManager::blueLedMode(true);}
    _powerManager->changeGsmPower(true);
    delay(GSM_WARMUP_DELAY_MS);

    PostData data = {
        .timestamp = _systemState.timestamp,

        .temperature = _currentWeather.temperature,
        .humidity = _currentWeather.humidity,
        .raindropLevel = _currentWeather.raindropLevel,
        .soilMoistureLevel = _currentWeather.soilMoistureLevel,

        .gsmSignalLevel = _webClient->lastSignalLevel,

        .solarVoltage = _currentPowerLevels.solarVoltage,
        .solarCurrent = _currentPowerLevels.solarCurrent,
        .batteryVoltage = _currentPowerLevels.batteryVoltage,
        .arduinoVoltage = _currentPowerLevels.arduinoVoltage,
        .gsmVoltage = _currentPowerLevels.gsmVoltage,

        .powerMode = (uint8_t)_systemState.powerMode,
        .restartsCount = _systemState.restartsCount,
        .gsmErrors = _systemState.gsmErrors
    };
    if(!_webClient->connect())_systemState.gsmErrors++;
    else {
        GetData gdata;
        if(!_webClient->postData(data, &gdata))_systemState.gsmErrors++;
        
        if(gdata.version != _settings._version){
            _settings.lightTimeSleepDurationSeconds = gdata.lightTimeSleepDurationSeconds;
            _settings.darkTimeSleepDurationSeconds = gdata.darkTimeSleepDurationSeconds;
            _settings.sendDataFrequency = gdata.sendDataFrequency;
            _settings.safeModeVoltage = gdata.safeModeVoltage;
            _settings.economyModeVoltage = gdata.economyModeVoltage;
            _settings._version = gdata.version;
            _storage->updateSettings(_settings);
        }

        _sendDataIterationCounter = 0;
        _webClient->disconnect();
    }

    _powerManager->changeGsmPower(false);
    if(_systemState.isDebugMode){ DebugModeManager::greenLedMode(false);DebugModeManager::blueLedMode(false);}
}

void LifecycleManager::updateSystemState(){    
    _systemState.timestamp = millis() / 1000;

    if(_currentPowerLevels.batteryVoltage <= _settings.safeModeVoltage)_systemState.powerMode = SystemPowerMode::Safe;
    else if(_currentPowerLevels.batteryVoltage <= _settings.economyModeVoltage)_systemState.powerMode = SystemPowerMode::Economy;
    else _systemState.powerMode = SystemPowerMode::Powerfull;

    #ifdef DEBUG
    Serial.print("LifecycleManager::updateSystemState powerMode=");Serial.println((int)_systemState.powerMode);
    #endif

    if(_systemState.isDebugMode){
        DebugModeManager::redLedMode(false);DebugModeManager::yellowLedMode(false);
        if(_systemState.powerMode == SystemPowerMode::Safe)DebugModeManager::redLedMode(true);
        else if(_systemState.powerMode == SystemPowerMode::Economy)DebugModeManager::yellowLedMode(true);
    }
}

void LifecycleManager::sleep(){
    if(_currentPowerLevels.solarVoltage >= _settings.solarVoltageForLightTime){
        _powerManager->deepSleep(_settings.lightTimeSleepDurationSeconds);
    }
    else {
        _powerManager->deepSleep(_settings.darkTimeSleepDurationSeconds);
    }
}