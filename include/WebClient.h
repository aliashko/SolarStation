#include "GsmClient.h"
#include "Models/GetData.h"
#include "Models/PostData.h"
#pragma once

class WebClient
{
public:
    WebClient();
    bool getUpdates(GetData* data);
    bool postData(PostData data);

private:
    GsmClient* _gsm;
};