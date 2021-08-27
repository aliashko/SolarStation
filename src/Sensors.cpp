#include "Sensors.h"
#include "config.h"

//#define DEBUG

Sensors::Sensors(){
    _dht = new SimpleDHT22(DHT22_PIN);
    _ina219 = new INA219_WE(INA_I2C_ADDRESS);
    pinMode(RAINDROP_PIN, INPUT);
    pinMode(SOIL_PIN, INPUT);
    Wire.begin();
}

bool Sensors::connect(){
    _isConnected = _ina219->init();
    _ina219->setMeasureMode(TRIGGERED);

    #ifdef DEBUG
    Serial.print("Sensors (INA219) connection: ");Serial.println(_isConnected);
    #endif

    return _isConnected;
}

Weather Sensors::getWeather(){
    Weather data;

    if (_dht->read2(&data.temperature, &data.humidity, NULL) != SimpleDHTErrSuccess){
        #ifdef DEBUG
        Serial.println("!!! DHT error");
        #endif
    }

    data.raindropLevel = 1023 - getDataFromAnalogPin(RAINDROP_PIN);
    data.soilMoistureLevel = 1023 - getDataFromAnalogPin(SOIL_PIN);
    
    #ifdef DEBUG
    Serial.print("Temp ");Serial.println(data.temperature);    
    Serial.print("Hum ");Serial.println(data.humidity);    
    Serial.print("Rain ");Serial.println(data.raindropLevel);    
    Serial.print("Soil ");Serial.println(data.soilMoistureLevel);    
    #endif

    return data;
}

PowerLevels Sensors::getPowerLevels(bool useOnlyBuiltinSensors){
    PowerLevels data;
    data.arduinoVoltage = getVoltageFromAnalogPin(ARDUINO_VOLTAGE_PIN, ARDUINO_VOLTMETER_R2, ARDUINO_VOLTMETER_R1)-0.25;
    data.batteryVoltage = getVoltageFromAnalogPin(BATTERY_VOLTAGE_PIN, BATTERY_VOLTMETER_R2, BATTERY_VOLTMETER_R1)-0.25;
    data.gsmVoltage = data.batteryVoltage;//getVoltageFromAnalogPin(GSM_VOLTAGE_PIN, GSM_VOLTMETER_R2, GSM_VOLTMETER_R1);
    
    if(!useOnlyBuiltinSensors){
        data.solarCurrent = _ina219->getCurrent_mA();
        data.solarVoltage = _ina219->getBusVoltage_V() + (_ina219->getShuntVoltage_mV() / 1000);
    }

    #ifdef DEBUG
    Serial.print("arduinoVoltage ");Serial.println(data.arduinoVoltage);    
    Serial.print("batteryVoltage ");Serial.println(data.batteryVoltage);    
    Serial.print("solarCurrent ");Serial.println(data.solarCurrent);    
    Serial.print("solarVoltage ");Serial.println(data.solarVoltage);    
    #endif

    return data;
}

float Sensors::getDataFromAnalogPin(uint8_t pin){
    float Vvalue=0.0;
    for(unsigned int i=0;i<10;i++){
        Vvalue=Vvalue+analogRead(pin); 
        delay(5);
    }
    return Vvalue=(float)Vvalue/10.0;
}

float Sensors::getVoltageFromAnalogPin(uint8_t pin, int r1, int r2){
    float Tvoltage=0.0;
    float Vvalue=0.0,Rvalue=0.0;
    float RatioFactor=(float)r2/(float)(r1+r2);

    for(unsigned int i=0;i<10;i++){
        Vvalue=Vvalue+analogRead(pin);         //Read analog Voltage
        delay(5);                              //ADC stable
    }

    Vvalue=(float)Vvalue/10.0;            //Find average of 10 values
    Rvalue=(float)(Vvalue/1023.0)*5;      //Convert Voltage in 5v factor
    Tvoltage=Rvalue/RatioFactor;          //Find original voltage by multiplying with factor

    return Tvoltage;
}