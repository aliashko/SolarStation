#pragma once

class DebugModeManager
{
public:
    static void initializeHardware();
    static bool checkIfDebugModeRequested();
    
    static void redLedMode(bool turnOn);
    static void greenLedMode(bool turnOn);
    static void blueLedMode(bool turnOn);
    static void yellowLedMode(bool turnOn);
    static void blinkRedLed();
    static void blinkAllLeds();    
};