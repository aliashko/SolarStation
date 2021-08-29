#include "Sensors.h"
#include "config.h"

//#define DEBUG

Sensors::Sensors(){
    _dht = new SimpleDHT22(DHT22_PIN);
    _ina219 = new INA219_WE(INA_I2C_ADDRESS);
    analogReference(EXTERNAL);
    pinMode(RAINDROP_PIN, INPUT);
    pinMode(SOIL_PIN, INPUT);
    pinMode(ARDUINO_VOLTAGE_PIN, INPUT);
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    Wire.begin();
}

bool Sensors::connect(){
    _isConnected = _ina219->init();
    _ina219->setADCMode(SAMPLE_MODE_128);

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

    analogReference(DEFAULT);
    delay(100);
    data.raindropLevel = 1023 - getDataFromAnalogPin(RAINDROP_PIN);
    data.soilMoistureLevel = 1023 - getDataFromAnalogPin(SOIL_PIN);
    analogReference(EXTERNAL);
    delay(100);
    
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
    float aref = 3.3 * 32000.0 / (float)(32000 + REF_RESISTOR);
    data.arduinoVoltage = getVoltageFromAnalogPin(ARDUINO_VOLTAGE_PIN, ARDUINO_VOLTMETER_R1, ARDUINO_VOLTMETER_R2, aref);
    data.batteryVoltage = getVoltageFromAnalogPin(BATTERY_VOLTAGE_PIN, BATTERY_VOLTMETER_R1, BATTERY_VOLTMETER_R2, aref);
    data.gsmVoltage = data.batteryVoltage;
    
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

float Sensors::getVoltageFromAnalogPin(uint8_t pin, long r1, long r2, float arefV){
    float RatioFactor=(float)r2/(float)(r1+r2);

    float Vvalue=getDataFromAnalogPin(pin);            //Find average of 10 values

    float Rvalue=1024.0 / (float)(Vvalue * arefV);      //Convert Voltage in 5v factor
    float Tvoltage=Rvalue/RatioFactor;          //Find original voltage by multiplying with factor

    return Tvoltage;
}