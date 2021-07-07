#include <SoftwareSerial.h>
#include <GsmClientConstants.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

// Set serial for debug console (to the Serial Monitor)
#define GSM_DEBUG true
#define GSM_SERIAL_MONITOR Serial
#define TINY_GSM_DEBUG SerialMonitor

class GsmClient
{
public:
    GsmClient(
        uint8_t rxPin, 
        uint8_t txPin, 
        const char* apn,
        const char* gprsUser, 
        const char* gprsPass, 
        const char* simPin = "");

    bool connect();
    void disconnect();
    void reset();

    bool sendRequest(String verb, String host, String resource, String body, String* response, int* httpCode);

private:
    bool _isConnected = false;

    uint8_t _rxPin;
    uint8_t _txPin;
    SoftwareSerial* _serialAT;
    const char* _apn;
    const char* _gprsUser;
    const char* _gprsPass;
    const char* _simPin;

    TinyGsm* _modem;
    TinyGsmClient* _client;
};