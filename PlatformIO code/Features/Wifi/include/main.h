#ifndef __MAIN_H__
#define __MAIN_H__

#include <Arduino.h>
#include "DHT.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DHTPIN 1
#define DHTTYPE DHT11
#define STACK_SIZE 4096
#define DEVICE_HOSTNAME "ESP32_4G_DEV"
#define TCP_SERVER_IP "47.92.146.210"
#define TCP_SERVER_PORT 8888


#endif