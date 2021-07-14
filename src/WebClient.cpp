#include "WebClient.h"
#include "config.h"

WebClient::WebClient(){
    GsmConfiguration config;
    _gsm = new GsmClient(
        SIM800_RX, SIM800_TX, 
        config.Apn, 
        config.GprsUser, 
        config.GprsPassword, 
        config.SimPin);
}

bool WebClient::getUpdates(GetData* data){
    if(!_gsm->connect())return false;

    BackendClientConfig config;
    String response; int httpCode;
    if(!_gsm->sendRequest("GET", config.Host, config.GetUpdatesEndpoint, "", &response, &httpCode) || httpCode != 200) {
        _gsm->disconnect();
        return false;
    }

    data->lightTimeSleepDurationInMinutes = readIntJsonField(&response, "lsd:");
    data->darkTimeSleepDurationInMinutes = readIntJsonField(&response, "dsd:");
    data->sendDataFrequency = readIntJsonField(&response, "sdf:");
    data->getDataFrequency = readIntJsonField(&response, "gdf:");
    data->safeModeVoltage = readFloatJsonField(&response, "smv:");
    data->economyModeVoltage = readFloatJsonField(&response, "emv:");
    data->smsInformNumber = readStringJsonField(&response, "sms:");

    _gsm->disconnect();
    return true;
}

bool WebClient::postData(PostData data){
    if(!_gsm->connect())return false;

    BackendClientConfig config;
    String request, response; int httpCode;

    request = String("{")+
                "ts:" + String(data.timestamp) +
                "t:" + String(data.temperature) +
                "h:" + String(data.humidity) +
                "r:" + String(data.raindropLevel) +
                "sv:" + String(data.solarVoltage) +
                "sc:" + String(data.solarCurrent) +
                "bv:" + String(data.batteryVoltage) +
                "av:" + String(data.arduinoVoltage) +
                "gv:" + String(data.gsmVoltage) +
                "is:" + String(data.isSafeMode) +
                "ie:" + String(data.isEconomyMode) +
                "rc:" + String(data.restartsCount) +
                "ge:" + String(data.gsmErrors) +
            +"}";

    if(!_gsm->sendRequest("POST", config.Host, config.PostDataEndpoint, request, &response, &httpCode) || httpCode != 200) {
        _gsm->disconnect();
        return false;
    }

    _gsm->disconnect();
    return true;
}

int WebClient::readIntJsonField(String* str, String field){
    auto strValue = readStringJsonField(str, field);
    if(strValue == "") return -1;
    return strValue.toInt();
}

float WebClient::readFloatJsonField(String* str, String field){
    auto strValue = readStringJsonField(str, field);
    if(strValue == "") return -1;
    return strValue.toFloat();
}

String WebClient::readStringJsonField(String* str, String field){
    int i = str->indexOf(field);
    if(i == -1) return "";
    int iend = str->indexOf(",", i);
    if(iend == -1)iend = str->indexOf("}", i);
    if(iend == -1) return "";
    if(iend <= i + (int)field.length()) return "";

    return str->substring(i + field.length(), iend);
}