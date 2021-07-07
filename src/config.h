#include <Arduino.h>

#ifndef CONFIG_H
#define CONFIG_H

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
const char* APN = "internet";
const char* GPRS_USER = "";
const char* GPRS_PASSWORD = "";

// BUTTONS
const uint8_t INTERRUPT_BUTTON_PIN = 2;

// DEBUG LED
const uint8_t RED_LED_PIN = 3;
const uint8_t GREEN_LED_PIN = 4;
const uint8_t BLUE_LED_PIN = 5;

struct BackendClientConfig {
    const char* Host = "http://solarstation.liashko.space";
    const char* PostDataEndpoint = "/api/postdata";
    const char* GetUpdatesEndpoint = "/api/getupdates";
};

#endif