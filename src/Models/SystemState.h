#pragma once

struct SystemState {
    unsigned long timestamp;
    bool isDebugMode;

    bool isSafeMode;
    bool isEconomyMode;

    unsigned long restartsCount;
    unsigned int gsmErrors;
};