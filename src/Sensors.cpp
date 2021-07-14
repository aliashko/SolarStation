#include "Sensors.h"
#include "config.h"

Sensors::Sensors(){
    _dht = new DHT(DHT22_PIN, DHT22);
    _ina219 = new Adafruit_INA219(INA_I2C_ADDRESS);
}

bool Sensors::connect(){
    _dht->begin();
    _isConnected = _ina219->begin();

    return _isConnected;
}

Weather Sensors::getWeather(){
    Weather data;
    data.temperature = _dht->readTemperature();
    data.humidity = _dht->readHumidity();

    return data;
}

PowerLevels Sensors::getPowerLevels(){
    PowerLevels data;
    data.solarCurrent = _ina219->getCurrent_mA();
    data.solarVoltage = _ina219->getBusVoltage_V() + (_ina219->getShuntVoltage_mV() / 1000);
    data.arduinoVoltage = getVoltageFromAnalogPin(ARDUINO_VOLTAGE_PIN, ARDUINO_VOLTMETER_R1, ARDUINO_VOLTMETER_R2);
    data.batteryVoltage = getVoltageFromAnalogPin(BATTERY_VOLTAGE_PIN, BATTERY_VOLTMETER_R1, BATTERY_VOLTMETER_R2);
    data.gsmVoltage = getVoltageFromAnalogPin(GSM_VOLTAGE_PIN, GSM_VOLTMETER_R1, GSM_VOLTMETER_R2);

    return data;
}

float Sensors::getVoltageFromAnalogPin(uint8_t pin, int r1, int r2){
    float Tvoltage=0.0;
    float Vvalue=0.0,Rvalue=0.0;
    float RatioFactor=(float)r2/(float)(r1+r2);

    for(unsigned int i=0;i<5;i++){
        Vvalue=Vvalue+analogRead(pin);         //Read analog Voltage
        delay(5);                              //ADC stable
    }

    Vvalue=(float)Vvalue/10.0;            //Find average of 10 values
    Rvalue=(float)(Vvalue/1023.0)*3.3;      //Convert Voltage in 3.3v factor
    Tvoltage=Rvalue/RatioFactor;          //Find original voltage by multiplying with factor

    return Tvoltage;
}