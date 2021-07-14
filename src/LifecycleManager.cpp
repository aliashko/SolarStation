#include "LifecycleManager.h"
#include "DebugModeManager.h"
#include "config.h"

LifecycleManager::LifecycleManager() {
    _powerManager = new PowerManager();
    _storage = new Storage();
    _sensors = new Sensors();
    _webClient = new WebClient();
}

void LifecycleManager::iterate() {
    _sendDataIterationCounter++;
    _getDataIterationCounter++;

    switch(_runtimeMode){
        case RuntimeMode::Initializating:
            initialize();
        break;
        case RuntimeMode::Measuring:
            measure();
            _runtimeMode = RuntimeMode::SendingData;
        break;
        case RuntimeMode::SendingData:
            sendData();
        break;
        case RuntimeMode::GettingUpdates:
            getUpdates();
        break;
        default:break;
    }
}

void LifecycleManager::initialize(){
    DebugModeManager::initializeHardware();

    _settings = _storage->getSettings();
    _systemState = SystemState{
        .timestamp = 0,
        .isDebugMode = DebugModeManager::checkIfDebugModeRequested(),
        .isSafeMode = false,
        .isEconomyMode = false,
        .restartsCount = _storage->getRestartsCount(),
        .gsmErrors = 0,
    };

    _storage->incrementRestartsCount();
}

void LifecycleManager::measure(){
    _powerManager->changeSensorsPower(true);
    delay(SENSORS_WARMUP_DELAY_MS);

    _sensors->connect();
    _currentWeather = _sensors->getWeather();
    _currentPowerLevels = _sensors->getPowerLevels();

    _powerManager->changeSensorsPower(false);
}

void LifecycleManager::sendData(){
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

        .isSafeMode = _systemState.isSafeMode,
        .isEconomyMode = _systemState.isEconomyMode,
        .restartsCount = _systemState.restartsCount,
        .gsmErrors = _systemState.gsmErrors
    };
    if(!_webClient->postData(data))_systemState.gsmErrors++;

    _powerManager->changeGsmPower(false);
}

void LifecycleManager::getUpdates(){
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
}