#include "WebClient.h"
#include "config.h"

WebClient::WebClient(){
    GsmConfiguration config;
    _gsm = new GsmClient(
        SIM800_RX, SIM800_TX, 
        config.Apn, 
        config.GprsUser, 
        config.GprsPassword,
        GSM_INITIALIZATION_DELAY_MS);
}

bool WebClient::getUpdates(GetData* data){
    if(!_gsm->connect())return false;

    BackendClientConfig config;
    String response; int httpCode;
    if(!_gsm->sendRequest("GET", config.Host, config.GetUpdatesEndpoint, NULL, &response, &httpCode) || httpCode != 200) {
        _gsm->disconnect();
        return false;
    }

    data->lightTimeSleepDurationInMinutes = readIntJsonField(&response, "lsd:");
    data->darkTimeSleepDurationInMinutes = readIntJsonField(&response, "dsd:");
    data->sendDataFrequency = readIntJsonField(&response, "sdf:");
    data->getDataFrequency = readIntJsonField(&response, "gdf:");
    data->safeModeVoltage = readFloatJsonField(&response, "smv:");
    data->economyModeVoltage = readFloatJsonField(&response, "emv:");
    data->economyModeDataSendSkipMultiplier = readIntJsonField(&response, "emm:");
    data->solarVoltageForLightTime = readFloatJsonField(&response, "svl:");    
    data->smsInformNumber = readStringJsonField(&response, "sms:");

    _gsm->disconnect();
    return true;
}

bool WebClient::postData(PostData data){
    if(!_gsm->connect())return false;

    BackendClientConfig config;
    String response; int httpCode;

    char request[200];char* buf;
    request[0] = '\0';

    strcat(request,"{");
    ultoa(data.timestamp,buf,15);jsonConcat(request, "ts", buf);
    itoa((int)(data.temperature*10),buf,10);jsonConcat(request, "t", buf);
    itoa((int)(data.humidity*10),buf,10);jsonConcat(request, "h", buf);
    utoa(data.raindropLevel,buf,10);jsonConcat(request, "r", buf);
    itoa((int)(data.solarVoltage*10),buf,10);jsonConcat(request, "sv", buf);
    itoa((int)(data.solarCurrent*10),buf,10);jsonConcat(request, "sc", buf);
    itoa((int)(data.batteryVoltage*10),buf,10);jsonConcat(request, "bv", buf);
    itoa((int)(data.arduinoVoltage*10),buf,10);jsonConcat(request, "av", buf);
    itoa((int)(data.gsmVoltage*10),buf,10);jsonConcat(request, "gv", buf);
    utoa(data.powerMode,buf,10);jsonConcat(request, "pm", buf);
    ultoa(data.restartsCount,buf,10);jsonConcat(request, "rc", buf);
    utoa(data.gsmErrors,buf,10);jsonConcat(request, "ge", buf);
    strcat(request,"}");

    if(!_gsm->sendRequest("POST", config.Host, config.PostDataEndpoint, request, &response, &httpCode) || httpCode != 200) {
        _gsm->disconnect();
        return false;
    }

    _gsm->disconnect();
    return true;
}

int WebClient::readIntJsonField(String* str, const char* field){
    auto strValue = readStringJsonField(str, field);
    if(strlen(strValue) == 0) return -1;

    return atoi(strValue);
}

float WebClient::readFloatJsonField(String* str, const char* field){
    auto intValue = readIntJsonField(str, field);
    if(intValue == -1) return -1;
    return (float)intValue/10.0;
}

const char* WebClient::readStringJsonField(String* str, const char* field){
    int i = str->indexOf(field);
    if(i == -1) return "";
    int iend = str->indexOf(",", i);
    if(iend == -1)iend = str->indexOf("}", i);
    if(iend == -1) return "";
    if(iend <= (int)(i + strlen(field))) return "";

    return (str->substring(i + strlen(field), iend)).c_str();
}

void WebClient::jsonConcat(char* json, const char* key, char* value){
    strcat(json, key);
    strcat(json,":");
    strcat(json, value);
    strcat(json,",");
}