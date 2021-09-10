#include "DebugModeManager.h"
#include "Arduino.h"
#include "config.h"

void DebugModeManager::initializeHardware(){
    pinMode(DEBUG_BUTTON_PIN, INPUT_PULLUP);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);
    pinMode(YELLOW_LED_PIN, OUTPUT);
}

bool DebugModeManager::checkIfDebugModeRequested(){
    for(int i = 0; i<5; i++){
        if(!digitalRead(DEBUG_BUTTON_PIN))return true;
        delay(10);
    }
    return false;
}

void DebugModeManager::redLedMode(bool turnOn){
    digitalWrite(RED_LED_PIN, turnOn);
}

void DebugModeManager::greenLedMode(bool turnOn){
    digitalWrite(GREEN_LED_PIN, turnOn);
}

void DebugModeManager::blueLedMode(bool turnOn){
    digitalWrite(BLUE_LED_PIN, turnOn);
}

void DebugModeManager::yellowLedMode(bool turnOn){
    digitalWrite(YELLOW_LED_PIN, turnOn);
}

void DebugModeManager::blinkAllLeds(){
    redLedMode(true);greenLedMode(true);blueLedMode(true);yellowLedMode(true);
    delay(2000);
    redLedMode(false);greenLedMode(false);blueLedMode(false);yellowLedMode(false);
}