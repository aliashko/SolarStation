#include "Sensors.h"
#include "config.h"

#define DEBUG
extern void safeDelay(unsigned int ms);

Sensors::Sensors(){
    _dht = new SimpleDHT22(DHT22_PIN);
    _ina219 = new INA219_WE(INA_I2C_ADDRESS);
    pinMode(RAINDROP_PIN, INPUT);
    pinMode(SOIL_PIN, INPUT);
    analogReference(INTERNAL);
    pinMode(ARDUINO_VOLTAGE_PIN, INPUT);
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    Wire.begin();
}

bool Sensors::connect(){
    _isConnected = _ina219->init();
    _ina219->setADCMode(SAMPLE_MODE_128);
    _ina219->setBusRange(BRNG_16);

    #ifdef DEBUG
    Serial.print(F("Sensors (INA219) connection: "));Serial.println(_isConnected);
    #endif

    return _isConnected;
}

Weather Sensors::getWeather(){
    Weather data;

    if (_dht->read2(&data.temperature, &data.humidity, NULL) != SimpleDHTErrSuccess){
        #ifdef DEBUG
        Serial.println(F("!!! DHT error"));
        #endif
    }

    analogReference(DEFAULT);
    safeDelay(SENSORS_WARMUP_DELAY_MS);
    data.raindropLevel = 1023 - (int)getDataFromAnalogPin(RAINDROP_PIN);
    data.soilMoistureLevel = 1023 - (int)getDataFromAnalogPin(SOIL_PIN);
    
    #ifdef DEBUG
    Serial.print(F("Temp "));Serial.println(data.temperature);    
    Serial.print(F("Hum "));Serial.println(data.humidity);    
    Serial.print(F("Rain "));Serial.println(data.raindropLevel);    
    Serial.print(F("Soil "));Serial.println(data.soilMoistureLevel);    
    #endif

    return data;
}

PowerLevels Sensors::getPowerLevels(bool useOnlyBuiltinSensors){
    PowerLevels data;
    float aref = 1.1;
    
    analogReference(INTERNAL);
    safeDelay(SENSORS_WARMUP_DELAY_MS);

    data.arduinoVoltage = getVoltageFromAnalogPin(ARDUINO_VOLTAGE_PIN, ARDUINO_VOLTMETER_RATIO, aref);
    data.batteryVoltage = getVoltageFromAnalogPin(BATTERY_VOLTAGE_PIN, BATTERY_VOLTMETER_RATIO, aref);
    data.gsmVoltage = data.batteryVoltage;
    data.solarCurrent = 0;
    data.solarVoltage = 0;
    
    if(!useOnlyBuiltinSensors){
        data.solarCurrent = _ina219->getCurrent_mA();
        data.solarVoltage = _ina219->getBusVoltage_V() + (_ina219->getShuntVoltage_mV() / 1000.0);
    }

    #ifdef DEBUG
    Serial.print(F("arduinoVoltage "));Serial.println(data.arduinoVoltage);    
    Serial.print(F("batteryVoltage "));Serial.println(data.batteryVoltage);    
    Serial.print(F("solarCurrent "));Serial.println(data.solarCurrent);    
    Serial.print(F("solarVoltage "));Serial.println(data.solarVoltage);    
    #endif

    return data;
}

float Sensors::getDataFromAnalogPin(uint8_t pin){
    float Vvalue=0.0;
    for(unsigned int i=0;i<30;i++){
        analogRead(pin); 
        delay(10);
    }    
    for(unsigned int i=0;i<256;i++){
        Vvalue=Vvalue+(float)analogRead(pin);
    }
    return Vvalue/256.0;
}

float Sensors::getVoltageFromAnalogPin(uint8_t pin, double ratioFactor, float arefV){
    //float RatioFactor=(float)r2/(float)(r1+r2);

    float Vvalue=getDataFromAnalogPin(pin);            //Find average of 10 values

    double Rvalue= (double)(Vvalue * arefV) / 1024.0;      //Convert Voltage in aref factor
    float Tvoltage=Rvalue/ratioFactor;          //Find original voltage by multiplying with factor

    return Tvoltage;
}