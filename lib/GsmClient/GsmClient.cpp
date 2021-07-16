#include "GsmClient.h"
#define POST_CONTENT_TYPE "application/json"

GsmClient::GsmClient(
	uint8_t rxPin, 
	uint8_t txPin, 
	const char* apn,
	const char* gprsUser, 
	const char* gprsPass,
	uint16_t initializationDelay){
		_rxPin = rxPin;
		_txPin = txPin;
		_apn = apn;
		_gprsUser = gprsUser;
		_gprsPass = gprsPass;
		_initializationDelay = initializationDelay;

		_serialAT = new SoftwareSerial(rxPin, txPin);
		_modem = new TinyGsm(*_serialAT);
		_client = new TinyGsmClient(*_modem);
}

bool GsmClient::connect(){
	// Set GSM module baud rate
	TinyGsmAutoBaud(*_serialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
	// _serialAT.begin(9600);
	delay(_initializationDelay);

	// Restart takes quite some time
	// To skip it, call init() instead of restart()
	#ifdef GSM_DEBUG 
	GSM_SERIAL_MONITOR.println("Initializing modem...");
	#endif
	_modem->restart();
	// _modem->init();

	#ifdef GSM_DEBUG 
	String modemInfo = _modem->getModemInfo();
	GSM_SERIAL_MONITOR.print("Modem Info: ");
	GSM_SERIAL_MONITOR.println(modemInfo);
	#endif
	// Unlock your SIM card with a PIN if needed
	//if ((_simPin != NULL) && (_simPin[0] != '\0') && _modem->getSimStatus() != 3) { _modem->simUnlock(_simPin); }

	#ifdef GSM_DEBUG 
	GSM_SERIAL_MONITOR.print("Waiting for network...");
	#endif
	if (!_modem->waitForNetwork()) {
		#ifdef GSM_DEBUG 
		GSM_SERIAL_MONITOR.println(" fail");
		#endif
		return false;
	}
	#ifdef GSM_DEBUG 
	GSM_SERIAL_MONITOR.println(" success");
	#endif

	#ifdef GSM_DEBUG  
	if (_modem->isNetworkConnected()) { 		
		GSM_SERIAL_MONITOR.println("Network connected"); 		
	}
	#endif
	
	// GPRS connection parameters are usually set after network registration
	#ifdef GSM_DEBUG 
	GSM_SERIAL_MONITOR.print(F("Connecting to "));
	GSM_SERIAL_MONITOR.print(_apn);
	#endif
	if (!_modem->gprsConnect(_apn, _gprsUser, _gprsPass)) {
		#ifdef GSM_DEBUG 
		GSM_SERIAL_MONITOR.println(" fail");
		#endif
		return false;
	}

	#ifdef GSM_DEBUG 
	GSM_SERIAL_MONITOR.println(" success");
	#endif

	if (_modem->isGprsConnected()) { 
		#ifdef GSM_DEBUG 
		GSM_SERIAL_MONITOR.println("GPRS connected"); 
		#endif
		_isConnected = true;
	}
	else _isConnected = false;

	return _isConnected;
}

void GsmClient::disconnect(){
	_modem->gprsDisconnect();
	//_modem->poweroff();
	#ifdef GSM_DEBUG 
	GSM_SERIAL_MONITOR.println(F("GPRS disconnected"));
	#endif
	_isConnected = false;
}

void GsmClient::reset(){
	_modem->restart();
}

bool GsmClient::sendRequest(const char* verb, const char* host, const char* resource, char* body, String* response, int* httpCode){
	HttpClient http(*_client, host);

	#ifdef GSM_DEBUG 
		GSM_SERIAL_MONITOR.print(F("Performing HTTP "));
		GSM_SERIAL_MONITOR.print(verb);
		GSM_SERIAL_MONITOR.print(F(" request... "));
	#endif
	
	if(strcmp(verb, "POST") == 0){
		http.post(resource, POST_CONTENT_TYPE, body);
	}
	else {
		http.get(resource);
	}

	*httpCode = http.responseStatusCode();
	#ifdef GSM_DEBUG 
	GSM_SERIAL_MONITOR.print(F("Response status code: "));
	GSM_SERIAL_MONITOR.println(*httpCode);
	#endif

	/*
	if(GSM_DEBUG) GSM_SERIAL_MONITOR.println(F("Response Headers:"));
	while (http.headerAvailable()) {
		String headerName  = http.readHeaderName();
		String headerValue = http.readHeaderValue();
		if(GSM_DEBUG) GSM_SERIAL_MONITOR.println("    " + headerName + " : " + headerValue);
	}
	*/

	#ifdef GSM_DEBUG
	int length = http.contentLength();
	if (length >= 0) {		
		GSM_SERIAL_MONITOR.print(F("Content length is: "));
		GSM_SERIAL_MONITOR.println(length);
		
	}
	if (http.isResponseChunked()) {
		GSM_SERIAL_MONITOR.println(F("The response is chunked"));
	}
	#endif

	*response = http.responseBody();

	#ifdef GSM_DEBUG
	GSM_SERIAL_MONITOR.println(F("Response:"));
	GSM_SERIAL_MONITOR.println(*response);
	GSM_SERIAL_MONITOR.print(F("Body length is: "));
	GSM_SERIAL_MONITOR.println((*response).length());
	#endif

	// Shutdown

	http.stop();
	#ifdef GSM_DEBUG
	GSM_SERIAL_MONITOR.println(F("Server disconnected"));
	#endif

	return true;
}