#pragma once
#include <Arduino.h>

const unsigned int SENSORS_WARMUP_DELAY_MS = 1000;
const unsigned int GSM_WARMUP_DELAY_MS = 1000;
const unsigned int GSM_INITIALIZATION_DELAY_MS = 5000;
const unsigned int GSM_OPERATIONS_DELAY_MS = 500;

const unsigned int HTTP_RESPONSE_BUFFER = 150;

// POWER (MOSFET CONTROLLING)
const uint8_t SENSORS_POWER_CONTROL_PIN = 2;
const uint8_t SIM800_POWER_CONTROL_PIN = 3;

// DHT22 5V VCC
const uint8_t DHT22_PIN = 4; //5V

// RAINDROP ANALOG 5V VCC
const uint8_t RAINDROP_PIN = A6; //5V

// INA219 (current sensor) 5V VCC
const uint8_t INA_I2C_ADDRESS = 0x40; //5V

// GSM (SIM800L) MODULE - 3.7-4.2V VCC (OR 5 for BLUE)
const uint8_t SIM800_TX = 10; //3.3V (OR 5 for BLUE)
const uint8_t SIM800_RX = 11; //3.3V (OR 5 for BLUE)
const uint8_t SIM800_RESET = 12; //3.3V (OR 5 for BLUE)

//#define SIM_PIN "0000"
struct GsmConfiguration {
    const char* Apn = "internet";
};

// VOLTAGE SENSORS SETTINGS
//const uint8_t SOLAR_VOLTAGE_PIN = A0;
const uint8_t BATTERY_VOLTAGE_PIN = A1;
const uint8_t ARDUINO_VOLTAGE_PIN = A2;
const uint8_t GSM_VOLTAGE_PIN = A3;
//const int SOLAR_VOLTMETER_R1 = 19400;
//const int SOLAR_VOLTMETER_R2 = 6740;
const unsigned int BATTERY_VOLTMETER_R1 = 19400;
const unsigned int BATTERY_VOLTMETER_R2 = 6740;
const unsigned int ARDUINO_VOLTMETER_R1 = 19400;
const unsigned int ARDUINO_VOLTMETER_R2 = 6740;
const unsigned int GSM_VOLTMETER_R1 = 19400;
const unsigned int GSM_VOLTMETER_R2 = 6740;

// BUTTONS
const uint8_t DEBUG_BUTTON_PIN = 5;

// DEBUG LED
const uint8_t RED_LED_PIN = 6;
const uint8_t YELLOW_LED_PIN = 7;
const uint8_t BLUE_LED_PIN = 8;
const uint8_t GREEN_LED_PIN = 9;

struct BackendClientConfig {
    const char* PostDataEndpoint = "http://solarstation.liashko.space/api/postdata";
    const char* GetUpdatesEndpoint = "http://solarstation.liashko.space/api/getupdates";
    const int GetUpdatesTimeout = 10000;
    const int PostDataTimeout = 10000;
};
