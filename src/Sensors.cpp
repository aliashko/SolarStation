#include "Sensors.h"
#include "config.h"

Sensors::Sensors(){
    _dht = new SimpleDHT22(DHT22_PIN);
    _ina219 = new INA219_WE(INA_I2C_ADDRESS);
}

bool Sensors::connect(){
    Wire.begin();
    _isConnected = _ina219->init();
    _ina219->setMeasureMode(TRIGGERED);
    return _isConnected;
}

Weather Sensors::getWeather(){
    Weather data;

    if (_dht->read2(&data.temperature, &data.humidity, NULL) != SimpleDHTErrSuccess);
    data.raindropLevel = 1023 - getDataFromAnalogPin(RAINDROP_PIN);

    return data;
}

PowerLevels Sensors::getPowerLevels(bool useOnlyBuiltinSensors){
    PowerLevels data;
    data.arduinoVoltage = 5.0;//getVoltageFromAnalogPin(ARDUINO_VOLTAGE_PIN, ARDUINO_VOLTMETER_R1, ARDUINO_VOLTMETER_R2);
    data.batteryVoltage = 5.0;//getVoltageFromAnalogPin(BATTERY_VOLTAGE_PIN, BATTERY_VOLTMETER_R1, BATTERY_VOLTMETER_R2);
    data.gsmVoltage = 5.0;//getVoltageFromAnalogPin(GSM_VOLTAGE_PIN, GSM_VOLTMETER_R1, GSM_VOLTMETER_R2);
    
    if(true || !useOnlyBuiltinSensors){
        data.solarCurrent = _ina219->getCurrent_mA();
        data.solarVoltage = _ina219->getBusVoltage_V() + (_ina219->getShuntVoltage_mV() / 1000);
    }

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