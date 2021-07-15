#pragma once
#include <Arduino.h>

const unsigned int SENSORS_WARMUP_DELAY_MS = 1000;
const unsigned int GSM_WARMUP_DELAY_MS = 1000;

// POWER (MOSFET CONTROLLING)
const uint8_t SENSORS_POWER_CONTROL_PIN = 0;
const uint8_t SIM800_POWER_CONTROL_PIN = 1;

// DHT22 5V VCC
const uint8_t DHT22_PIN = 0; //5V

// INA219 (current sensor) 5V VCC
const uint8_t INA_I2C_ADDRESS = 0x40; //5V

// GSM (SIM800L) MODULE - 3.7-4.2V VCC
const uint8_t SIM800_TX = 6; //3.3V
const uint8_t SIM800_RX = 7; //3.3V
struct GsmConfiguration {
    const char* SimPin = "";
    const char* Apn = "internet";
    const char* GprsUser = "";
    const char* GprsPassword = "";
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
const uint8_t DEBUG_BUTTON_PIN = 2;

// DEBUG LED
const uint8_t RED_LED_PIN = 3;
const uint8_t GREEN_LED_PIN = 4;
const uint8_t BLUE_LED_PIN = 5;
const uint8_t YELLOW_LED_PIN = 8;

struct BackendClientConfig {
    const char* Host = "http://solarstation.liashko.space";
    const char* PostDataEndpoint = "/api/postdata";
    const char* GetUpdatesEndpoint = "/api/getupdates";
};
