#include "GsmClient.h"
#include "GyverWDT.h"

extern void safeDelay(unsigned int ms);

#define POST_CONTENT_TYPE "application/json"
const uint8_t MAX_RETRY_COUNT = 10;
const uint8_t MAX_CONNECTION_RETRY_COUNT = 3;

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
		_sim800 = new SIM800L((Stream *)_simSerial, _resetPin, 150, _httpResponseBuffer);
		#else
		_sim800 = new SIM800L((Stream *)_simSerial, _resetPin, 150, _httpResponseBuffer, (Stream *)&Serial);
		#endif		
}
bool GsmClient::connect(){
	sleepMode(false);
	_sim800->setBaudRate(9600);
	for(int i=0;i<MAX_CONNECTION_RETRY_COUNT;i++){
		if(connectInternal())return true;
		reset();
		Watchdog.reset();
		safeDelay(_initializationDelay);
	}
	return false;
}

bool GsmClient::connectInternal(){
	#ifdef GSM_DEBUG 
	GSM_SERIAL_MONITOR.begin(9600);
	GSM_SERIAL_MONITOR.println("Initializing modem...");
	#endif

	safeDelay(_initializationDelay);

	// Wait until the module is ready to accept AT commands
	for(uint8_t i = 0; !_sim800->isReady(); i++) {
		#ifdef GSM_DEBUG 
		Serial.println(F("Problem to initialize AT command, retry in 1 sec"));
		#endif
		safeDelay(_operationsDelay);
		Watchdog.reset();
		if(i >= MAX_RETRY_COUNT) return false;
	}
	#ifdef GSM_DEBUG 
	Serial.println(F("Setup Complete!"));
	#endif

	// Wait for the GSM signal
	currentSignalLevel = _sim800->getSignal();
	for(uint8_t i = 0; currentSignalLevel <= 0; i++) {
		safeDelay(_operationsDelay);
		Watchdog.reset();
		currentSignalLevel = _sim800->getSignal();
		if(i >= MAX_RETRY_COUNT) return false;
	}
	#ifdef GSM_DEBUG 
	Serial.print(F("Signal OK (strenght: "));
	Serial.print(currentSignalLevel);
	Serial.println(F(")"));
	#endif
	safeDelay(_operationsDelay);
	Watchdog.reset();

	// Wait for operator network registration (national or roaming network)
	NetworkRegistration network = _sim800->getRegistrationStatus();
	for(uint8_t i = 0; network != REGISTERED_HOME && network != REGISTERED_ROAMING; i++) {
		safeDelay(_operationsDelay);
		Watchdog.reset();
		network = _sim800->getRegistrationStatus();
		if(i >= MAX_RETRY_COUNT) return false;
	}
	#ifdef GSM_DEBUG 
	Serial.println(F("Network registration OK"));
	safeDelay(_operationsDelay);
	#endif

	// Setup APN for GPRS configuration
	bool success = _sim800->setupGPRS(_apn);
	for(uint8_t i = 0; !success; i++){
		success = _sim800->setupGPRS(_apn);
		safeDelay(_operationsDelay);
		Watchdog.reset();
		if(i >= MAX_RETRY_COUNT) return false;
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
		safeDelay(_operationsDelay);	
		Watchdog.reset();	
	}
	
	#ifdef GSM_DEBUG 
	if(disconnected) {
		Serial.println(F("GPRS disconnected !"));
	} else {
		Serial.println(F("GPRS still connected !"));
	}
	#endif

	// Go into low power mode
	sleepMode(true);

	_isConnected = !disconnected;
}

void GsmClient::reset(){
	#ifdef GSM_DEBUG 
	GSM_SERIAL_MONITOR.println("reset...");
	#endif
	_sim800->reset();
}

void GsmClient::sleepMode(bool sleepModeOn){
	if(sleepModeOn){
		powerModeInternal(true);
		sleepModeInternal(true);
	}
	else {
		//_sim800->reset();
		//safeDelay(_operationsDelay);
		sleepModeInternal(false);
		safeDelay(_operationsDelay);
		powerModeInternal(false);
		safeDelay(_operationsDelay);
	}
}

void GsmClient::powerModeInternal(bool lowPowerMode){
	for(int i=0; i<2;i++){
		bool powerMode = _sim800->setPowerMode(lowPowerMode ? SLEEP : NORMAL);		
		#ifdef GSM_DEBUG 
		if(powerMode) {
			Serial.print(F("Power mode isLow="));Serial.println(lowPowerMode);
		} else {
			Serial.print(F("Failed to switch power mode to isLow="));Serial.println(lowPowerMode);
		}
		#endif
		delay(100);
		if(powerMode)break;
	}
}
void GsmClient::sleepModeInternal(bool sleepModeOn){
	for(int i=0; i<2;i++){
		bool sleepMode = _sim800->setSleepMode(sleepModeOn);		
		#ifdef GSM_DEBUG 
		if(sleepMode) {
			Serial.print(F("Sleep mode turned to "));Serial.println(sleepModeOn);
		} else {
			Serial.print(F("Failed to switch sleep mode to "));Serial.println(sleepModeOn);
		}
		#endif
		delay(100);
		if(sleepMode)break;
	}
}

bool GsmClient::sendRequest(const char* verb, const char* url, char* body, int timeout, char* response, int* httpCode){
	// Establish GPRS connectivity (5 trials)
	bool connected = false;
	for(uint8_t i = 0; i < 5 && !connected; i++) {
		connected = _sim800->connectGPRS();
		safeDelay(_operationsDelay);
		Watchdog.reset();
	}
	safeDelay(_operationsDelay);
	Watchdog.reset();

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
	if(strcmp(verb, "GET") == 0)/**httpCode = _sim800->doGet(url, timeout)*/;
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
