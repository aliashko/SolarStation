#include <Arduino.h>
#include <PowerManager.h>
#include <config.h>

PowerManager::PowerManager(){
    pinMode(SENSORS_POWER_CONTROL_PIN, OUTPUT);
    digitalWrite(SENSORS_POWER_CONTROL_PIN, isSensorsEnabled);
    pinMode(SIM800_POWER_CONTROL_PIN, OUTPUT);
    digitalWrite(SIM800_POWER_CONTROL_PIN, isGsmEnabled);
}

void PowerManager::deepSleep(int seconds){
    power.sleepDelay(1000 * seconds);
}

void PowerManager::wakeUp(){
    power.wakeUp();
}

void PowerManager::changeSensorsPower(bool enable){
    isSensorsEnabled = enable;
    digitalWrite(SENSORS_POWER_CONTROL_PIN, isSensorsEnabled);
}

void PowerManager::changeGsmPower(bool enable){
    isGsmEnabled = enable;
    digitalWrite(SIM800_POWER_CONTROL_PIN, isGsmEnabled);
}