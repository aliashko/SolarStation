#pragma once

class DebugModeManager
{
public:
    static void initializeHardware();
    static bool checkIfDebugModeRequested();
    static void redLedMode(bool turnOn);
    static void greenLedMode(bool turnOn);
    static void blueLedMode(bool turnOn);
};