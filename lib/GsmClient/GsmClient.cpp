#include "GsmClient.h"
#define POST_CONTENT_TYPE "application/json"

GsmClient::GsmClient(
	uint8_t rxPin, 
	uint8_t txPin, 
	uint8_t resetPin, 
	const char* apn,
	uint16_t initializationDelay,
	uint16_t operationsDelay,
	uint16_t httpResponseBuffer){
		_rxPin = rxPin;
		_txPin = txPin;
		_resetPin = resetPin;
		_apn = apn;
		_initializationDelay = initializationDelay;
		_operationsDelay = operationsDelay;
		_httpResponseBuffer = httpResponseBuffer;

		_simSerial = new SoftwareSerial(rxPin, txPin);
		_simSerial->begin(9600);
		#ifndef SIM800L_INTERNAL_DEBUG 
		_sim800 = new SIM800L((Stream *)_simSerial, _resetPin, 200, _httpResponseBuffer);
		#else
		_sim800 = new SIM800L((Stream *)_simSerial, _resetPin, 200, _httpResponseBuffer, (Stream *)&Serial);
		#endif
}

bool GsmClient::connect(){
	#ifdef GSM_DEBUG 
	GSM_SERIAL_MONITOR.begin(9600);
	GSM_SERIAL_MONITOR.println("Initializing modem...");
	#endif
	//reset();
	delay(_initializationDelay);

	// Wait until the module is ready to accept AT commands
	while(!_sim800->isReady()) {
		#ifdef GSM_DEBUG 
		Serial.println(F("Problem to initialize AT command, retry in 1 sec"));
		#endif
		delay(_operationsDelay);
	}
	#ifdef GSM_DEBUG 
	Serial.println(F("Setup Complete!"));
	#endif

	// Wait for the GSM signal
	currentSignalLevel = _sim800->getSignal();
	while(currentSignalLevel <= 0) {
		delay(_operationsDelay);
		currentSignalLevel = _sim800->getSignal();
	}
	#ifdef GSM_DEBUG 
	Serial.print(F("Signal OK (strenght: "));
	Serial.print(currentSignalLevel);
	Serial.println(F(")"));
	#endif
	delay(_operationsDelay);

	// Wait for operator network registration (national or roaming network)
	NetworkRegistration network = _sim800->getRegistrationStatus();
	while(network != REGISTERED_HOME && network != REGISTERED_ROAMING) {
		delay(_operationsDelay);
		network = _sim800->getRegistrationStatus();
	}
	#ifdef GSM_DEBUG 
	Serial.println(F("Network registration OK"));
	delay(_operationsDelay);
	#endif

	// Setup APN for GPRS configuration
	bool success = _sim800->setupGPRS(_apn);
	while(!success){
		success = _sim800->setupGPRS(_apn);
		delay(_operationsDelay);
	}
	#ifdef GSM_DEBUG 
	Serial.println(F("GPRS config OK"));
	#endif

	_isConnected = success;
	return _isConnected;
}

void GsmClient::disconnect(){
	// Close GPRS connectivity (5 trials)
	bool disconnected = _sim800->disconnectGPRS();
	for(uint8_t i = 0; i < 5 && !disconnected; i++) {
		disconnected = _sim800->disconnectGPRS();
		delay(_operationsDelay);		
	}
	
	#ifdef GSM_DEBUG 
	if(disconnected) {
		Serial.println(F("GPRS disconnected !"));
	} else {
		Serial.println(F("GPRS still connected !"));
	}
	#endif
/*
	// Go into low power mode
	bool lowPowerMode = _sim800->setPowerMode(MINIMUM);
	#ifdef GSM_DEBUG 
	if(lowPowerMode) {
		Serial.println(F("Module in low power mode"));
	} else {
		Serial.println(F("Failed to switch module to low power mode"));
	}
	#endif
*/
	_isConnected = !disconnected;
}

void GsmClient::reset(){
	_sim800->reset();
}

bool GsmClient::sendRequest(const char* verb, const char* url, char* body, int timeout, char* response, int* httpCode){
	// Establish GPRS connectivity (5 trials)
	bool connected = false;
	for(uint8_t i = 0; i < 5 && !connected; i++) {
		connected = _sim800->connectGPRS();
		delay(_operationsDelay);
	}
	delay(_operationsDelay);

	// Check if connected, if not reset the module and setup the config again
	if(connected) {
		#ifdef GSM_DEBUG 
		Serial.println(F("GPRS connected"));
		#endif
	} else {
		#ifdef GSM_DEBUG 
		Serial.println(F("GPRS not connected !"));
		Serial.println(F("Reset the module."));
		#endif
		return false;
	}

	#ifdef GSM_DEBUG 
	Serial.print(F("Start HTTP "));
	Serial.println(verb);
	Serial.println(url);
	Serial.println(body);
	#endif

	// Do HTTP communication
	if(strcmp(verb, "GET") == 0)*httpCode = _sim800->doGet(url, timeout);
	else *httpCode = _sim800->doPost(url, POST_CONTENT_TYPE, body, timeout, timeout);

	if(*httpCode == 200) {
		// Success
		#ifdef GSM_DEBUG 
		Serial.print(F("HTTP successful ("));
		Serial.print(_sim800->getDataSizeReceived());
		Serial.println(F(" bytes)"));
		#endif
		strcpy(response, _sim800->getDataReceived());
		#ifdef GSM_DEBUG 
		Serial.print(F("Received : "));
		Serial.println(response);
		#endif
	} else {
		// Failed...
		#ifdef GSM_DEBUG 
		Serial.print(F("HTTP error "));
		Serial.println(*httpCode);
		#endif
		return false;
	}	
	return true;
}
/*

bool GsmClient::sendRequest2(const char* verb, const char* url, char* body, int timeout){
	// Establish GPRS connectivity (5 trials)
	bool connected = false;
	for(uint8_t i = 0; i < 5 && !connected; i++) {
		connected = _sim800->connectGPRS();
		delay(_operationsDelay);
	}
	delay(_operationsDelay);

	// Check if connected, if not reset the module and setup the config again
	if(connected) {
		#ifdef GSM_DEBUG 
		Serial.println(F("GPRS connected"));
		#endif
	} else {
		#ifdef GSM_DEBUG 
		Serial.println(F("GPRS not connected !"));
		Serial.println(F("Reset the module."));
		#endif
		return false;
	}

	#ifdef GSM_DEBUG 
	Serial.print(F("Start HTTP "));
	Serial.println(verb);
	Serial.println(url);
	Serial.println(body);
	#endif

	// Do HTTP communication
	int httpCode;char response[_httpResponseBuffer];
	if(strcmp(verb, "GET") == 0)httpCode = _sim800->doGet(url, timeout);
	else httpCode = _sim800->doPost(url, POST_CONTENT_TYPE, body, timeout, timeout);

	if(httpCode == 200) {
		// Success
		#ifdef GSM_DEBUG 
		Serial.print(F("HTTP successful ("));
		Serial.print(_sim800->getDataSizeReceived());
		Serial.println(F(" bytes)"));
		#endif
		strcpy(response, _sim800->getDataReceived());
		//response = _sim800->getDataReceived();		
		#ifdef GSM_DEBUG 
		Serial.print(F("Received : "));
		Serial.println(response);
		#endif
	} else {
		// Failed...
		#ifdef GSM_DEBUG 
		Serial.print(F("HTTP error "));
		Serial.println(httpCode);
		#endif
		return false;
	}	
	return true;
}*/