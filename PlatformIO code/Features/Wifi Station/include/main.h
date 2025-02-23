#ifndef __MAIN_H__
#define __MAIN_H__

// Required libraries
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char *ssid = "ESP32S3_AP_TEST";   // WiFi network name
const char *password = "1234567890";    // WiFi password
#define  LED_RED  8                 // GPIO pin for RED LED


#endif