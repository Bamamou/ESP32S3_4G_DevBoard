#ifndef __MAIN_H__
#define __MAIN_H__

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include "Website.h"

// DHT11 configuration
// DHT11 Temperature and Humidity Sensor Configuration
// DHTPIN: GPIO pin where the sensor is connected
#define DHTPIN 1
#define DHTTYPE DHT11

// WiFi Network Credentials
// Replace with your network credentials
const char* ssid = "nicolas";
const char* password = "Jesuisde94.";

#endif