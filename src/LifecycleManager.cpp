#include "LifecycleManager.h"
#include "DebugModeManager.h"
#include "config.h"
#include "GyverWDT.h"

extern void safeDelay(unsigned int ms);
//#define DEBUG

LifecycleManager::LifecycleManager() {    
    DebugModeManager::initializeHardware();
    DebugModeManager::blinkRedLed();

    _powerManager = new PowerManager();
    _storage = new Storage();
    _sensors = new Sensors();
    _webClient = new WebClient();

    initialize();
}

void LifecycleManager::initialize(){
    _systemState = SystemState{
        .timestamp = 0,
        .isDebugMode = DebugModeManager::checkIfDebugModeRequested(),
        .powerMode = SystemPowerMode::Safe,
        .restartsCount = 0,
        .gsmErrors = 0
    };

    if(_systemState.isDebugMode)DebugModeManager::blinkAllLeds();
    else DebugModeManager::blinkRedLed();
    if(DebugModeManager::checkIfDebugModeRequested()){
        _settings = _storage->getDefaultSettings();
        _storage->updateRestartsCount(0);
        DebugModeManager::blinkRedLed(2);
    }
    else _settings = _storage->getSettings();

    if(_settings._integrityControlKey == SETTINGS_INTEGRITY_CONTROL_KEY_VALUE){
        _systemState.restartsCount = _storage->getRestartsCount();
    }
    else {
        #ifdef DEBUG
        Serial.print(F(">>> settings INTEGRITY mismatch "));Serial.println(_settings._integrityControlKey);
        #endif
        _settings._integrityControlKey = SETTINGS_INTEGRITY_CONTROL_KEY_VALUE;
        _storage->updateSettings(_settings);
    }

    if(!_systemState.isDebugMode)_storage->updateRestartsCount(_systemState.restartsCount + 1);
}

void LifecycleManager::iterate() {
    #ifdef DEBUG
    Serial.println(F("LifecycleManager::iterate"));
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
    Serial.println(F("LifecycleManager::measure"));
    #endif
    if(_systemState.isDebugMode) DebugModeManager::greenLedMode(true);
    _powerManager->changeSensorsPower(true);
    Watchdog.reset();

    _sensors->connect();

    _currentPowerLevels = _sensors->getPowerLevels();
    Watchdog.reset();
    _currentWeather = _sensors->getWeather();
  
    _powerManager->changeSensorsPower(false);
    if(_systemState.isDebugMode) DebugModeManager::greenLedMode(false);
}

void LifecycleManager::measureForSafeMode(){
    #ifdef DEBUG
    Serial.println(F("LifecycleManager::measureForSafeMode"));
    #endif

    _currentPowerLevels = _sensors->getPowerLevels(true);
}

void LifecycleManager::sendData(){
    #ifdef DEBUG
    Serial.println(F("LifecycleManager::sendData"));
    #endif

    if(_systemState.isDebugMode)DebugModeManager::blueLedMode(true);
    _powerManager->changeGsmPower(true);
    delay(GSM_WARMUP_DELAY_MS);

    if(!_webClient->connect()){
        #ifdef DEBUG
        Serial.println(F("sendData connection error"));
        #endif
        _systemState.gsmErrors++;
        if(_settings.resetSendDataCounterAfterFailure || _systemState.powerMode == SystemPowerMode::Economy)_sendDataIterationCounter = 0;
        if(_systemState.isDebugMode){
            if(_webClient->lastSignalLevel == -1)DebugModeManager::blinkRedBlueLed(3);
            else if(_webClient->lastSignalLevel == 0)DebugModeManager::blinkRedBlueLed(5);
            else DebugModeManager::blinkRedLed(3);
            Watchdog.reset();
        }
        _webClient->sleepMode(true);
    }
    else {
        #ifdef DEBUG
        Serial.println(F("sendData connection success"));
        #endif
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
            .gsmVoltage = _webClient->lastSIMVoltage,
            .powerMode = (uint8_t)_systemState.powerMode,

            .simMoneyBalance = -1,

            .restartsCount = _systemState.restartsCount,
            .gsmErrors = _systemState.gsmErrors
        };

        if(_systemState.powerMode == SystemPowerMode::Powerfull && _sendSupplementDataIterationCounter >= _settings.sendSupplementalDataFrequency){
            data.simMoneyBalance = _webClient->getBalance();
            _sendSupplementDataIterationCounter = 0;
            Watchdog.reset();
        }

        GetData gdata;
        if(!_webClient->postData(data, &gdata)){
            #ifdef DEBUG
            Serial.println(F("sendData post error"));
            #endif
            _systemState.gsmErrors++;
            _webClient->resetDevice();
            Watchdog.reset();
            safeDelay(GSM_OPERATIONS_DELAY_MS);
            if(_settings.resetSendDataCounterAfterFailure)_sendDataIterationCounter = 0;
            if(_systemState.isDebugMode){DebugModeManager::blinkRedLed(5);Watchdog.reset();}
        }
        else {
            #ifdef DEBUG
            Serial.println(F("sendData post success"));
            #endif
            Settings newSettings = {
                .lightTimeSleepDurationSeconds = gdata.lightTimeSleepDurationSeconds,
                .darkTimeSleepDurationSeconds = gdata.darkTimeSleepDurationSeconds,
                .sendDataFrequency = gdata.sendDataFrequency,
                .sendSupplementalDataFrequency = gdata.sendSupplementalDataFrequency,
                .resetSendDataCounterAfterFailure = gdata.resetSendDataCounterAfterFailure,

                .safeModeVoltage = gdata.safeModeVoltage,
                .economyModeVoltage = gdata.economyModeVoltage,
                .economyModeDataSendSkipMultiplier = gdata.economyModeDataSendSkipMultiplier,
                .solarVoltageForLightTime = gdata.solarVoltageForLightTime,

                ._version = gdata.version,
                ._integrityControlKey = _settings._integrityControlKey
            };

            if(newSettings._version != _settings._version){
                if(isSettingsValid(newSettings)){
                    _settings = newSettings;
                    _storage->updateSettings(_settings);
                }
                else {
                    #ifdef DEBUG
                    Serial.println("newSettings is invalid");
                    #endif
                }
            }
            _sendDataIterationCounter = 0;            
            _sendSupplementDataIterationCounter++;
        }

        _webClient->disconnect();
    }

    _powerManager->changeGsmPower(false);
    if(_systemState.isDebugMode)DebugModeManager::blueLedMode(false);
}

void LifecycleManager::updateSystemState(){    
    _systemState.timestamp = millis() / 1000;

    if(_currentPowerLevels.batteryVoltage <= _settings.safeModeVoltage)_systemState.powerMode = SystemPowerMode::Safe;
    else if(_currentPowerLevels.batteryVoltage <= _settings.economyModeVoltage)_systemState.powerMode = SystemPowerMode::Economy;
    else _systemState.powerMode = SystemPowerMode::Powerfull;

    #ifdef DEBUG
    Serial.print(F("updateSystemState powerMode="));Serial.println((int)_systemState.powerMode);
    #endif

    if(_systemState.isDebugMode){
        DebugModeManager::redLedMode(false);DebugModeManager::yellowLedMode(false);
        if(_systemState.powerMode == SystemPowerMode::Safe)DebugModeManager::redLedMode(true);
        else if(_systemState.powerMode == SystemPowerMode::Economy)DebugModeManager::yellowLedMode(true);
    }
}

void LifecycleManager::sleep(){
    if(_systemState.powerMode == SystemPowerMode::Safe){
        _powerManager->deepSleep(SAFE_MODE_SLEEP_DURATION);
        return;
    }

    if(_currentPowerLevels.solarVoltage >= _settings.solarVoltageForLightTime){
        _powerManager->deepSleep(_settings.lightTimeSleepDurationSeconds);
    }
    else {
        _powerManager->deepSleep(_settings.darkTimeSleepDurationSeconds);
    }
}