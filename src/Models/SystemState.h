#pragma once

enum class PowerMode {
    Unknown,
    Powerfull,
    Economy,
    Safe
};

struct SystemState {
    unsigned long timestamp;
    bool isDebugMode;
    PowerMode powerMode;

    unsigned long restartsCount;
    unsigned int gsmErrors;
};