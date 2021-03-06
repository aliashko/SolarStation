#pragma once
#include <Arduino.h>

const unsigned int SENSORS_WARMUP_DELAY_MS = 3000;
const unsigned int GSM_WARMUP_DELAY_MS = 1000;
const unsigned int GSM_INITIALIZATION_DELAY_MS = 6000;
const unsigned int GSM_OPERATIONS_DELAY_MS = 1000;
const unsigned int POWER_SWITCH_STABILIZATION_DELAY = 2000;
const unsigned int SAFE_MODE_SLEEP_DURATION = 300;

const unsigned int HTTP_RESPONSE_BUFFER = 96;
const unsigned int USSD_RESPONSE_BUFFER = 40;

// POWER (MOSFET CONTROLLING)
const uint8_t SENSORS_POWER_CONTROL_PIN = 11;
const uint8_t SIM800_POWER_CONTROL_PIN = 12; // DISABLED PHUSICALLY

// METEO SENSORS 5V VCC
const uint8_t DHT22_PIN = 7; //5V
const uint8_t EXTERNAL_DHT_PIN = 13; //5V
const uint8_t RAINDROP_PIN = A6; //5V
const uint8_t SOIL_PIN = A7; //5V

// INA219 (current sensor) 5V VCC
const uint8_t INA_I2C_ADDRESS = 0x40; //5V

// GSM (SIM800L) MODULE - 3.7-4.2V VCC (OR 5 for BLUE)
const uint8_t SIM800_RX = 8; //3.3V (OR 5 for BLUE)
const uint8_t SIM800_TX = 9; //3.3V (OR 5 for BLUE)
const uint8_t SIM800_RESET = 10; //3.3V (OR 5 for BLUE)

//#define SIM_PIN "0000"
struct GsmConfiguration {
    const char* Apn = "internet";
    const char* CheckBalanceUssd = "*111#";
};

// VOLTAGE SENSORS SETTINGS
const uint8_t BATTERY_VOLTAGE_PIN = A2;
const uint8_t ARDUINO_VOLTAGE_PIN = A3;
const double BATTERY_VOLTMETER_RATIO = 0.0826;//0.0834 - measurement attempt 2 result
const double ARDUINO_VOLTMETER_RATIO = 0.0844;//0.0855 - measurement attempt 2 result

// BUTTONS
const uint8_t DEBUG_BUTTON_PIN = 2;

// DEBUG LED
const uint8_t GREEN_LED_PIN = 3;
const uint8_t YELLOW_LED_PIN = 4;
const uint8_t BLUE_LED_PIN = 5;
const uint8_t RED_LED_PIN = 6;

struct BackendClientConfig {
    const char* PostDataEndpoint = "http://solarstation.liashko.space/api/postdata";
    const int PostDataTimeout = 10000;
};
