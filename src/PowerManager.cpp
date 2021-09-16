#include <Arduino.h>
#include <PowerManager.h>
#include <config.h>
#include "GyverWDT.h"

//#define DEBUG

PowerManager::PowerManager(){
    power.autoCalibrate();
    pinMode(SENSORS_POWER_CONTROL_PIN, OUTPUT);
    digitalWrite(SENSORS_POWER_CONTROL_PIN, isSensorsEnabled);
    pinMode(SIM800_POWER_CONTROL_PIN, OUTPUT);
    digitalWrite(SIM800_POWER_CONTROL_PIN, isGsmEnabled);
}

void PowerManager::deepSleep(int seconds){//seconds=10;
    #ifdef DEBUG
    Serial.print("PowerManager::deepSleep ");Serial.println(seconds);
    #endif

    safeDelay(1000 * (uint32_t)seconds);
    Watchdog.enable(RESET_MODE, WDT_PRESCALER_1024);
}

void PowerManager::safeDelay(uint32_t ms){
    for(int i = 0; i < 50; i++)delay(10);
	for(uint32_t i = 0; i < ms; i += 1000){
        power.sleepDelay(ms - i > 1000 ? 1000 : (ms - i));
        //delay((ms - i) > 1000 ? 1000 : (ms - i));
		Watchdog.reset();
	}
    for(int i = 0; i < 50; i++)delay(10);
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