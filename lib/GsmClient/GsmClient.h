#pragma once
#include <SoftwareSerial.h>
#include <GsmClientConstants.h>
#include "SIM800L.h"

// Set serial for debug console (to the Serial Monitor)
#define GSM_DEBUG
#ifdef GSM_DEBUG
    #define GSM_SERIAL_MONITOR Serial
    //#define SIM800L_INTERNAL_DEBUG
#endif

class GsmClient
{
public:
    GsmClient(
        uint8_t rxPin, 
        uint8_t txPin, 
        uint8_t resetPin, 
        const char* apn,
        uint16_t initializationDelay = 6000,
        uint16_t operationsDelay = 1000,
        uint16_t httpResponseBuffer = 256);

    bool connect();
    void disconnect();
    void reset();

    bool sendRequest(const char* verb, const char* url, char* body, int timeout, char* response, int* httpCode);    
    //bool sendRequest2(const char* verb, const char* url, char* body, int timeout);    
    int currentSignalLevel = -1;
private:
    bool _isConnected = false;

    uint8_t _rxPin;
    uint8_t _txPin;
    uint8_t _resetPin;
    SoftwareSerial* _simSerial;
    const char* _apn;
    uint16_t _initializationDelay;
    uint16_t _operationsDelay;
    uint16_t _httpResponseBuffer;

    SIM800L* _sim800;
};