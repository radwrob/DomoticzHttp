/*
 ============================================================================
Name      : config.h
Author    : Radoslaw Wrobel radoslaw.wrobel@gmx.com
Version   : 1.0
Copyright : BSD
 ============================================================================
*/

#ifndef config_H
#define config_H

#include <SPI.h>
#include <Ethernet2.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

//#define DEBUG

#ifdef DEBUG
  #define Sbegin(a) (Serial.begin(a))
  #define Sprintln(a) (Serial.println(a))
  #define Sprint(a) (Serial.print(a))
#else
  #define Sbegin(a)
  #define Sprintln(a) 
  #define Sprint(a) 
#endif
//ARDUINO GPIO
#define ONE_WIRE_BUS 2
#define GPIO_ALARM 3
#define GPIO_SABOT_ALARM 5
#define GPIO_SIREN 6
#define GPIO_SABOT_SIREN 7
#define GPIO_DHT11 8
#define BUTTON 9
#define LED_RED 14
#define LED_YELLOW 15
#define LED_GREEN 16
#define A_BATTERY A3
#define GPIO_LIGHT 19
//DOMOTICZ IDX
#define ALARM_IDX 4
#define SABOT_ALARM_IDX 5
#define SIREN_IDX 6
#define SABOT_SIREN_IDX 7
#define TEMP_YELLOW_IDX 8
#define TEMP_PACKAGE_IDX 9
#define TEMP_BLACK_IDX 10
#define LIGHT_IDX 11
#define DHT11_IDX 12
#define LED_GREEN_IDX 13
#define LED_YELLOW_IDX 14
#define LED_RED_IDX 15
#define ALARM_INDICATOR_IDX 16
#define VOLTAGE_IDX 17
#define ATTACK_INDICATOR_IDX 18
//DOMOTICZ VARIABLES
#define LIGHT_VAR 1
#define ALARM_INDICATOR_VAR 2
#define ATTACK_INDICATOR_VAR 3
#define SIREN_VAR 4
#define SABOT_SIREN_VAR 5
#define ALARM_VAR 6
#define SABOT_ALARM_VAR 7
#define LED_GREEN_VAR 8
#define LED_YELLOW_VAR 9
#define LED_RED_VAR 10

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 100);
IPAddress server(192, 168, 1, 1);
const char USERNAME[] = "base64_user";
const char PASSWORD[] = "base64_pass";
const unsigned long HTTP_TIMEOUT = 1000;  // max respone time from server

// Addresses of 3 DS18B20s
uint8_t sensor_black[8] = { 0x28, 0xFF, 0x54, 0xE6, 0x62, 0x17, 0x04, 0x8B };
uint8_t sensor_yellow[8] = { 0x28, 0xFF, 0xF6, 0x24, 0x74, 0x16, 0x04, 0x5A };
uint8_t sensor_package[8] = { 0x28, 0xFF, 0x05, 0x0F, 0xB2, 0x17, 0x05, 0x4E };

EthernetClient client;
DHT dht;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

unsigned long previousMillis = 0;           // will store last time temperature was updated
const unsigned long interval = 120000;      // interval at which to send temperature to Domoticz (milliseconds)
unsigned long previousMillisAlarm = 0;      // will store last time alarm was updated
const unsigned long intervalAlarm = 15000;  // interval at which to disarm alarm (milliseconds)
unsigned long previousMillisSwitch = 0;      // will store last time switch was updated
const unsigned long intervalSwitch = 30000;  // interval at which to read switch (milliseconds)

#endif /*config_H*/
