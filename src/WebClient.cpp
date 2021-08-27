#include "WebClient.h"
#include "config.h"
#include "GyverWDT.h"

WebClient::WebClient(){
    GsmConfiguration config;
    _gsm = new GsmClient(
        SIM800_RX, SIM800_TX, SIM800_RESET,
        config.Apn,
        GSM_INITIALIZATION_DELAY_MS,
        GSM_OPERATIONS_DELAY_MS,
        HTTP_RESPONSE_BUFFER);
}

bool WebClient::connect(){
    if(!_gsm->connect())return false;
    Watchdog.reset();
    lastSignalLevel = _gsm->currentSignalLevel;
    return true;
}

bool WebClient::disconnect(){
    _gsm->disconnect();
    Watchdog.reset();
    return true;
}

bool WebClient::postData(PostData data, GetData* gdata){
    BackendClientConfig config;
    char response[HTTP_RESPONSE_BUFFER]; int httpCode;

    char request[150];char buf[12];
    request[0] = '\0';

    strcat(request,"{");
    ultoa(data.timestamp,buf,10);jsonConcat(request, "ts", buf);
    itoa((int)(data.temperature*10),buf,10);jsonConcat(request, "t", buf);
    itoa((int)(data.humidity*10),buf,10);jsonConcat(request, "h", buf);
    utoa(data.raindropLevel,buf,10);jsonConcat(request, "r", buf);
    utoa(data.soilMoistureLevel,buf,10);jsonConcat(request, "sm", buf);
    utoa(data.gsmSignalLevel,buf,10);jsonConcat(request, "gl", buf);
    itoa((int)(data.solarVoltage*100),buf,10);jsonConcat(request, "sv", buf);
    itoa((int)(data.solarCurrent*100),buf,10);jsonConcat(request, "sc", buf);
    itoa((int)(data.batteryVoltage*100),buf,10);jsonConcat(request, "bv", buf);
    itoa((int)(data.arduinoVoltage*100),buf,10);jsonConcat(request, "av", buf);
    itoa((int)(data.gsmVoltage*100),buf,10);jsonConcat(request, "gv", buf);
    utoa(data.powerMode,buf,10);jsonConcat(request, "pm", buf);
    ultoa(data.restartsCount,buf,10);jsonConcat(request, "rc", buf);
    utoa(data.gsmErrors,buf,10);jsonConcat(request, "ge", buf);
    strcat(request,"}");

    if(!_gsm->sendRequest("POST", config.PostDataEndpoint, request, config.PostDataTimeout, response, &httpCode)) {
        //delete[] response;
        Watchdog.reset();
        return false;
    }

    int intValue = -1;int floatValue = -1;long longValue = -1;
    intValue = readIntJsonField(response, "lsd:");if(intValue != -1)gdata->lightTimeSleepDurationInMinutes = intValue;
    intValue = readIntJsonField(response, "dsd:");if(intValue != -1)gdata->darkTimeSleepDurationInMinutes = intValue;
    intValue = readIntJsonField(response, "sdf:");if(intValue != -1)gdata->sendDataFrequency = intValue;

    floatValue = readFloatJsonField(response, "smv:");if(floatValue != -1)gdata->safeModeVoltage = floatValue;
    floatValue = readFloatJsonField(response, "emv:");if(floatValue != -1)gdata->economyModeVoltage = floatValue;
    intValue = readIntJsonField(response, "emm:");if(intValue != -1)gdata->economyModeDataSendSkipMultiplier = intValue;
    floatValue = readFloatJsonField(response, "svl:");if(floatValue != -1)gdata->solarVoltageForLightTime = floatValue;

    longValue = readLongJsonField(response, "sms:");if(longValue != -1)gdata->smsInformNumber = longValue;
    Watchdog.reset();

    //delete[] response;
    Watchdog.reset();
    return true;
}

int WebClient::readIntJsonField(char* str, const char* field){
    char strValue[11];
    if(!readStringJsonField(str, field, strValue))return -1;

    auto intValue = atoi(strValue);

    return intValue;
}

long WebClient::readLongJsonField(char* str, const char* field){
    char strValue[11];
    if(!readStringJsonField(str, field, strValue))return -1;

    auto intValue = atol(strValue);

    return intValue;
}

float WebClient::readFloatJsonField(char* str, const char* field){
    auto intValue = readIntJsonField(str, field);
    if(intValue == -1) return -1;
    return (float)((float)intValue/10.0);
}

bool WebClient::readStringJsonField(char* str, const char* field, char* returnValue){    
    char* fieldStart = strstr(str, field);
    if(fieldStart == NULL) return false;

    char* fieldEnd = strchr(fieldStart, ',');
    if(fieldEnd == NULL) fieldEnd = strchr(fieldStart, '}');
    if(fieldEnd == NULL) return false;
    if(fieldEnd <= fieldStart + strlen(field)) return false;

    char* valueStart = fieldStart + strlen(field);
    strncpy(returnValue, valueStart, fieldEnd - valueStart);
    returnValue[fieldEnd - valueStart] = '\0';

    return true;
}

void WebClient::jsonConcat(char* json, const char* key, char* value){
    strcat(json, key);
    strcat(json,":");
    strcat(json, value);
    strcat(json,",");
}