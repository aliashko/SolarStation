#pragma once
#include "Arduino.h"

class DebugModeManager
{
public:
    static void initializeHardware();
    static bool checkIfDebugModeRequested();
    
    static void redLedMode(bool turnOn);
    static void greenLedMode(bool turnOn);
    static void blueLedMode(bool turnOn);
    static void yellowLedMode(bool turnOn);
    static void blinkRedLed(uint8_t count = 1);
    static void blinkRedBlueLed(uint8_t count = 1);
    static void blinkAllLeds();    
};