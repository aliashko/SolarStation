#include <Arduino.h>
#include <PowerManager.h>
#include <config.h>
#include "GyverWDT.h"

//#define DEBUG

PowerManager::PowerManager(){
    pinMode(SENSORS_POWER_CONTROL_PIN, OUTPUT);
    changeSensorsPower(isSensorsEnabled);
    pinMode(SIM800_POWER_CONTROL_PIN, OUTPUT);
    changeGsmPower(isGsmEnabled);
}

void PowerManager::deepSleep(int seconds){
    #ifdef DEBUG
    Serial.print("PowerManager::deepSleep ");Serial.println(seconds);
    #endif

    safeDelay(1000 * (seconds));
}

void PowerManager::safeDelay(unsigned int ms){
	for(unsigned int i = 0; i < ms; i += 1000){
        //power.sleepDelay(ms - i > 1000 ? 1000 : (ms - i));
        delay(ms - i > 1000 ? 1000 : (ms - i));
		Watchdog.reset();
	}
}

void PowerManager::wakeUp(){
    power.wakeUp();
}

void PowerManager::changeSensorsPower(bool enable){
    isSensorsEnabled = enable;
    digitalWrite(SENSORS_POWER_CONTROL_PIN, isSensorsEnabled);
    delay(POWER_SWITCH_STABILIZATION_DELAY);
}

void PowerManager::changeGsmPower(bool enable){
    isGsmEnabled = enable;
    digitalWrite(SIM800_POWER_CONTROL_PIN, !isGsmEnabled);
    //delay(POWER_SWITCH_STABILIZATION_DELAY);
}