#pragma once
#include "GsmClient.h"
#include "Models/GetData.h"
#include "Models/PostData.h"

class WebClient
{
public:
    WebClient();
    bool connect();
    bool disconnect();
    void resetDevice();
    void sleepMode(bool sleepModeOn);
    bool postData(PostData data, GetData* gdata);

    int lastSignalLevel = -1;

private:
    GsmClient* _gsm;

    int readIntJsonField(char* str, const char* field);
    long readLongJsonField(char* str, const char* field);
    float readFloatJsonField(char* str, const char* field);
    bool readStringJsonField(char* str, const char* field, char* returnValue);

    void jsonConcat(char* json, const char* key, char* value);
};