#pragma once
#include "GsmClient.h"
#include "Models/GetData.h"
#include "Models/PostData.h"

class WebClient
{
public:
    WebClient();
    bool getUpdates(GetData* data);
    bool postData(PostData data);

private:
    GsmClient* _gsm;

    int readIntJsonField(String* str, const char* field);
    float readFloatJsonField(String* str, const char* field);
    const char* readStringJsonField(String* str, const char* field);

    void jsonConcat(char* json, const char* key, char* value);
};