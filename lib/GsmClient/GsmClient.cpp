#include "GsmClient.h"
#define POST_CONTENT_TYPE "application/json"

GsmClient::GsmClient(
	uint8_t rxPin, 
	uint8_t txPin, 
	const char* apn,
	const char* gprsUser, 
	const char* gprsPass, 
	const char* simPin){
		_rxPin = rxPin;
		_txPin = txPin;
		_apn = apn;
		_gprsUser = gprsUser;
		_gprsPass = gprsPass;
		_simPin = simPin;
		_serialAT = new SoftwareSerial(rxPin, txPin);
}

bool GsmClient::connect(){
	if(_modem != NULL) delete(_modem);
	if(_client != NULL) delete(_client);
	
	_modem = new TinyGsm(*_serialAT);
	_client = new TinyGsmClient(*_modem);

	// Set GSM module baud rate
	TinyGsmAutoBaud(*_serialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
	// _serialAT.begin(9600);
	delay(6000);

	// Restart takes quite some time
	// To skip it, call init() instead of restart()
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println("Initializing modem...");
	_modem->restart();
	// _modem->init();

	String modemInfo = _modem->getModemInfo();
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.print("Modem Info: ");
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(modemInfo);

	// Unlock your SIM card with a PIN if needed
	if (_simPin != "" && _modem->getSimStatus() != 3) { _modem->simUnlock(_simPin); }

	if(GSM_DEBUG) GSM_SERIAL_MONITOR.print("Waiting for network...");
	if (!_modem->waitForNetwork()) {
		if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(" fail");
		return false;
	}
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(" success");

	if (_modem->isNetworkConnected()) { if(GSM_DEBUG) GSM_SERIAL_MONITOR.println("Network connected"); }

	// GPRS connection parameters are usually set after network registration
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.print(F("Connecting to "));
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.print(_apn);
	if (!_modem->gprsConnect(_apn, _gprsUser, _gprsPass)) {
		if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(" fail");
		return false;
	}
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(" success");

	if (_modem->isGprsConnected()) { 
		if(GSM_DEBUG) GSM_SERIAL_MONITOR.println("GPRS connected"); 
		_isConnected = true;
	}
	else _isConnected = false;

	return _isConnected;
}

void GsmClient::disconnect(){
	_modem->gprsDisconnect();
	//_modem->poweroff();
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(F("GPRS disconnected"));
	_isConnected = false;
}

void GsmClient::reset(){
	_modem->restart();
}

bool GsmClient::sendRequest(String verb, String host, String resource, String body, String* response, int* httpCode){
	HttpClient http(*_client, host);

	if(GSM_DEBUG) {
		GSM_SERIAL_MONITOR.print(F("Performing HTTP "));
		GSM_SERIAL_MONITOR.print(verb);
		GSM_SERIAL_MONITOR.print(F(" request... "));
	}
	int err;
	
	if(verb == "GET"){
		err = http.get(resource);
	}
	else if(verb == "POST"){
		err = http.post(resource, POST_CONTENT_TYPE, body);
	}
	if (err != 0) {
		if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(F("failed to connect"));
		return false;
	}

	*httpCode = http.responseStatusCode();
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.print(F("Response status code: "));
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(*httpCode);

	/*
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(F("Response Headers:"));
	while (http.headerAvailable()) {
		String headerName  = http.readHeaderName();
		String headerValue = http.readHeaderValue();
		if(GSM_DEBUG) GSM_SERIAL_MONITOR.println("    " + headerName + " : " + headerValue);
	}
	*/

	int length = http.contentLength();
	if (length >= 0) {
		if(GSM_DEBUG) GSM_SERIAL_MONITOR.print(F("Content length is: "));
		if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(length);
	}
	if (http.isResponseChunked()) {
		if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(F("The response is chunked"));
	}

	*response = http.responseBody();
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(F("Response:"));
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(*response);

	if(GSM_DEBUG) GSM_SERIAL_MONITOR.print(F("Body length is: "));
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println((*response).length());

	// Shutdown

	http.stop();
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(F("Server disconnected"));
}