#pragma once

enum class SystemPowerMode {
    Unknown,
    Powerfull,
    Economy,
    Safe
};

struct SystemState {
    unsigned long timestamp;
    bool isDebugMode;
    SystemPowerMode powerMode;

    unsigned long restartsCount;
    unsigned long gsmErrors;
};