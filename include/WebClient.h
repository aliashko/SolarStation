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

    int readIntJsonField(String* str, String field);
    float readFloatJsonField(String* str, String field);
    String readStringJsonField(String* str, String field);
};