/*
 ============================================================================
Name      : DomoticzHttp
Author    : Radoslaw Wrobel radoslaw.wrobel@gmx.com
Version   : 1.0
Copyright : BSD
 ============================================================================
*/

#include "config.h"
//watchdog
#include <avr/wdt.h>

uint8_t attackState = 0;
uint8_t alarmIndicatorState = 0;
uint8_t alarmState = 0;
uint8_t sabotAlarmState = 0; 
uint8_t sabotSirenState = 0;
uint8_t sirenState = 0;
uint8_t lightState = 0;
uint8_t redState = 0;
uint8_t yellowState = 0;
uint8_t greenState = 0;
uint8_t relayStateTmp = 0;
uint8_t acSignal = 0;
uint8_t btnState = 0;
bool alarmTrigerred = false;

char payload_buf[64] = {0};

void callback() {
  char buffer[100] = {0};
  char *pch;
  int i = 0;
  bool status_flag = false; 

  //read response from Domoticz
  client.readBytesUntil('\r', buffer, sizeof(buffer));
  if (strcmp(buffer, "HTTP/1.1 200 OK") != 0) { 
    Sprint(F("Unexpected response: "));
    return;      
  }    

  while(client.available()){
    while(client.available() && (i < sizeof(buffer))) {
      buffer[i] = client.read();
      if(buffer[i] == '\n'){ 
        break;
      } else {
        i++;
      }
    }
    buffer[i++] = 0;
    //Sprintln(buffer); 
    pch = strstr( buffer, "\"status\" : \"OK\"" );
    if( pch ) {  
      status_flag = true;
    }
    for(int x=0; x<i; x++)
      buffer[x] = 0;
    i = 0;      
  }
  
  if(!status_flag) {  
    Sprintln("Error response");
    return;
  }  
}

void switch_callback() {
  char buffer[100] = {0};
  char *pch;
  int i = 0;
  int idx;
  uint8_t value;
  bool idx_flag = false, val_flag = false, status_flag = false; 

  //read response from Domoticz
  client.readBytesUntil('\r', buffer, sizeof(buffer));
  if (strcmp(buffer, "HTTP/1.1 200 OK") != 0) { 
    Sprint(F("Unexpected response: "));
    return;      
  }    

  while(client.available()){
    while(client.available() && (i < sizeof(buffer))) {
      buffer[i] = client.read();
      if(buffer[i] == '\n'){ 
        break;
      } else {
        i++;
      }
    }
    buffer[i++] = 0;
    //Sprintln(buffer);
    pch = strstr( buffer, "\"swiatlo_var\"" );
    if( pch ) {    
      idx = LIGHT_IDX;
      Sprint("swiatlo_var ");
    }
    pch = strstr( buffer, "\"alarm_var\"" );
    if( pch ) {    
      idx = ALARM_INDICATOR_IDX;
      Sprint("alarm_var ");
    }
    pch = strstr( buffer, "\"napad_var\"" );
    if( pch ) {    
      idx = ATTACK_INDICATOR_IDX;
      Sprint("napad_var ");
    }
    pch = strstr( buffer, "\"syrena_var\"" );
    if( pch ) {    
      idx = SIREN_IDX;
      Sprint("syrena_var ");
    }        
    pch = strstr( buffer, "\"sabot_syrena_var\"" );
    if( pch ) {    
      idx = SABOT_SIREN_IDX;
      Sprint("sabot_syrena_var ");
    }  
    pch = strstr( buffer, "\"motion_var\"" );
    if( pch ) {    
      idx = ALARM_IDX;
      Sprint("motion_var ");
    }  
    pch = strstr( buffer, "\"sabot_motion_var\"" );
    if( pch ) {    
      idx = SABOT_ALARM_IDX;
      Sprint("sabot_motion_var ");
    }  
    pch = strstr( buffer, "\"led_green_var\"" );
    if( pch ) {    
      idx = LED_GREEN_IDX;
      Sprint("led_green_var ");
    }  
    pch = strstr( buffer, "\"led_yellow_var\"" );
    if( pch ) {    
      idx = LED_YELLOW_IDX;
      Sprint("led_yellow_var ");
    }  
    pch = strstr( buffer, "\"led_red_var\"" );
    if( pch ) {    
      idx = LED_RED_IDX;
      Sprint("led_red_var ");
    }                  
        
    pch = strstr( buffer, "Value" );
    if( pch ) {
      value = atoi(pch + 10);
      Sprintln(value);
      val_flag = true;
    }  
    pch = strstr( buffer, "\"status\" : \"OK\"" );
    if( pch ) {  
      status_flag = true;
    }
    for(int x=0; x<i; x++)
      buffer[x] = 0;
    i = 0;      
  }
  
  if(!status_flag) {  
    Sprintln("Error response");
    return;
  } 
  if(!val_flag) {
    Sprintln("switch value not found in payload");
    return;
  }
  switch ( idx )
  {
     case LIGHT_IDX:
        lightState = value;
        break;
     case LED_GREEN_IDX:
        greenState = value;
        break;
     case LED_RED_IDX:
        redState = value;
        break;
     case LED_YELLOW_IDX:
        yellowState = value;
        break;
     case SIREN_IDX:
        sirenState = value;
        break;        
     case SABOT_SIREN_IDX:
        sabotSirenState = value;
        break;
     case ALARM_IDX:
        alarmState = value;
        break;
     case SABOT_ALARM_IDX:
        sabotAlarmState = value;
        break;
     case ALARM_INDICATOR_IDX:
        alarmIndicatorState = value;
        break;   
     case ATTACK_INDICATOR_IDX:
        attackState = value;
        if( attackState ){
          greenState = 1;
          yellowState = 1;
          redState = 1;  
          sirenState = 1;
          lightState = 1;
          changeAllGpio();    
        }else{
          sirenState = 0;
          changeGpioState(GPIO_SIREN, LOW); 
        }        
        break;                                                               
     default:
        Sprintln("Undefined IDX");
  } 
}

void httpRequest(char payload[]){
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  uint8_t status = client.connect(server, 80);
  if (status == 1) {
    Sprintln("connected");
    // Make a HTTP request:
    client.print("GET /json.htm?username=");
    client.print(USERNAME);
    client.print("&password=");
    client.print(PASSWORD);
    client.print(payload);
    client.println(" HTTP/1.1");
    client.println("Host: www.arduino.cc");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Sprint("connection failed, error code: ");
    Sprintln(status);
  }
}

void setup()
{
  Sbegin(9600);
  
  wdt_enable(WDTO_8S);
  
  sensors.begin();
  dht.setup(GPIO_DHT11);
  
  pinMode(GPIO_ALARM, INPUT_PULLUP);
  pinMode(GPIO_SABOT_ALARM, INPUT_PULLUP);
  pinMode(GPIO_SABOT_SIREN, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(GPIO_SIREN, OUTPUT);
  pinMode(GPIO_LIGHT, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  if( Ethernet.begin(mac) == 0 ) {
    Sprintln("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  client.setTimeout(HTTP_TIMEOUT);
  Sprint("Arduino IP address: ");
  Sprintln(Ethernet.localIP());
  
  // Allow the hardware to sort itself out
  delay(1500);
  
  changeAllGpio();
  sendTemperature();    
  sendDHT11Value();
  checkDomoticzAllSwitchStatus();
}

void loop()
{   
  wdt_reset();
  
  // Read digital motion value
  unsigned long currentMillis = millis();
  relayStateTmp = digitalRead(GPIO_ALARM);
  if( relayStateTmp != alarmState ){
    alarmState = relayStateTmp;
    Sprintln("Alarm: " + String(alarmState)); 
    changeSwitchState( ALARM_IDX, alarmState );
    if ((alarmIndicatorState == 1) && (alarmState == 1) && (attackState == 0)) {
      alarmTrigerred = true;    
      armedAlarm(alarmIndicatorState, alarmTrigerred);  
      previousMillisAlarm = currentMillis;
    }
  }
  relayStateTmp = digitalRead(GPIO_SABOT_ALARM);
  if( relayStateTmp != sabotAlarmState ){
    sabotAlarmState = relayStateTmp;
    Sprintln("Sabotage Alarm: " + String(sabotAlarmState)); 
    changeSwitchState( SABOT_ALARM_IDX, sabotAlarmState );      
  }
  relayStateTmp = digitalRead(GPIO_SABOT_SIREN);
  if( relayStateTmp != sabotSirenState ){
    sabotSirenState = relayStateTmp;
    Sprintln("Sabotage Siren: " + String(sabotSirenState)); 
    changeSwitchState( SABOT_SIREN_IDX, sabotSirenState );      
  }    

  //TEMPERATURE  
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendTemperature();    
    sendDHT11Value();
    reportAcSignal();  
    getDeviceInfo(LED_GREEN_VAR);
    wdt_reset();
    getDeviceInfo(LED_YELLOW_VAR);
    wdt_reset();
    getDeviceInfo(LED_RED_VAR);
    wdt_reset();      
  }

  //MOTION DETECT
  currentMillis = millis();
  if(alarmTrigerred){
    if (currentMillis - previousMillisAlarm >= intervalAlarm) {
      alarmTrigerred = false;
      if( alarmIndicatorState ){
        attackState = 1;
        attack(attackState);
      }
    }  
  }

  //ALARM BUTTON
  relayStateTmp = digitalRead(BUTTON);
  if( relayStateTmp != btnState ){
    //button debounce delay
    delay(100);
    relayStateTmp = digitalRead(BUTTON);
    if( relayStateTmp != btnState ){  
      btnState = relayStateTmp;
      if( alarmIndicatorState || attackState ){
        alarmIndicatorState = 0;
        armedAlarm(alarmIndicatorState,0);    
        changeGpioState(GPIO_SIREN, HIGH);
        delay(100);       
        changeGpioState(GPIO_SIREN, LOW);
        delay(900);   
        wdt_reset();                     
      }else {
        alarmIndicatorState = 1;
        armedAlarm(alarmIndicatorState,0);
        for(int i=0;i<10;i++){
          changeGpioState(GPIO_SIREN, HIGH);
          delay(100);       
          changeGpioState(GPIO_SIREN, LOW);
          delay(900);   
          wdt_reset();       
        }
      }
      attackState = 0;
      attack(attackState);
    }
  }
  
  //SWITCH
  currentMillis = millis();
  if (currentMillis - previousMillisSwitch >= intervalSwitch) {
    previousMillisSwitch = currentMillis;  
    getDeviceInfo(ATTACK_INDICATOR_VAR);
    wdt_reset();
    getDeviceInfo(ALARM_INDICATOR_VAR);
    wdt_reset();
    getDeviceInfo(LIGHT_VAR);
    wdt_reset();
    getDeviceInfo(SIREN_VAR);
    wdt_reset();
  }
  
  updateAcSignal();
  changeAllGpio();
}

void updateAcSignal(){
  //check L signal on power supply
  uint8_t tmp = ( analogRead(A_BATTERY) < 800 ) ? 12 : 230;
  if(tmp != acSignal){
    acSignal = tmp;
    reportAcSignal();
  }
}

void reportAcSignal(){
  Sprintln("AC: " + String(acSignal));
  changeDomoticzSingleValue(VOLTAGE_IDX, acSignal);
}

void attack(uint8_t enable) {
  changeSwitchState(ATTACK_INDICATOR_IDX, enable);  
  if( enable ){
    greenState = 1;
    yellowState = 1;
    redState = 1;  
    sirenState = 1;
    lightState = 1;
    changeAllGpio(); 
    changeSwitchState(LED_GREEN_IDX,greenState);
    changeSwitchState(LED_YELLOW_IDX,yellowState);
    changeSwitchState(LED_RED_IDX,redState);     
    changeSwitchState(SIREN_IDX,sirenState);     
    changeSwitchState(LIGHT_IDX,lightState);     
  }else{
    sirenState = 0;
    changeGpioState(GPIO_SIREN, LOW);
    changeSwitchState(SIREN_IDX,sirenState); 
  }
}

void armedAlarm(uint8_t enableAlarm, uint8_t trigerred){
  if(trigerred){
    greenState = 0;
    yellowState = 1;
    redState = 0;    
  }else if(enableAlarm){
    greenState = 0;
    yellowState = 0;
    redState = 1;
  }else{
    greenState = 1;
    yellowState = 0;
    redState = 0;    
  }
  changeAllGpio();
  //update domoticz state
  changeSwitchState(ALARM_INDICATOR_IDX, enableAlarm);  
  changeSwitchState(LED_GREEN_IDX,greenState);
  changeSwitchState(LED_YELLOW_IDX,yellowState);
  changeSwitchState(LED_RED_IDX,redState);
}

void changeSwitchState(int idx, uint8_t value){
  char tmp[5];
  payload_buf[0] = 0;
  strcat(payload_buf,"&type=command&param=udevice&idx=");
  sprintf(tmp, "%d", idx);
  strcat(payload_buf,tmp);
  strcat(payload_buf,"&nvalue=");
  sprintf(tmp, "%d", value);
  strcat(payload_buf,tmp);
  strcat(payload_buf,"&svalue=0");   
  Sprintln(payload_buf);
  httpRequest(payload_buf);
  callback();
}

void changeDomoticzSingleValue(int idx, float value){
  char tmp[10]; 
  uint8_t pos = 0;
  payload_buf[0] = 0;
  strcat(payload_buf,"&type=command&param=udevice&idx=");
  sprintf(tmp, "%d", idx);
  strcat(payload_buf,tmp);
  strcat(payload_buf,"&nvalue=0&svalue=");
  dtostrf(value, 9, 2, tmp);
  //delete redundant space
  while(tmp[pos] == ' '){
    if (pos < sizeof(tmp)) pos++;
    else break;
  }
  strcat(payload_buf,&tmp[pos]); 
  Sprintln(payload_buf);
  httpRequest(payload_buf);
  callback();
}

void changeDomoticzDoubleValue(int idx, float temp, float humid){
  char tmp[10];
  uint8_t pos = 0;
  payload_buf[0] = 0;
  strcat(payload_buf,"&type=command&param=udevice&idx=");
  sprintf(tmp, "%d", idx);
  strcat(payload_buf,tmp);
  strcat(payload_buf,"&nvalue=0&svalue=");
  dtostrf(temp, 9, 2, tmp);
  //delete redundant space
  while(tmp[pos] == ' '){
    if (pos < sizeof(tmp)) pos++;
    else break;
  }
  strcat(payload_buf,&tmp[pos]);
  strcat(payload_buf,";");
  dtostrf(humid, 9, 2, tmp);
  //delete redundant space
  while(tmp[pos] == ' '){
    if (pos < sizeof(tmp)) pos++;
    else break;
  }
  strcat(payload_buf,&tmp[pos]);  
  strcat(payload_buf,";2");  
  Sprintln(payload_buf);
  httpRequest(payload_buf);
  callback();
}

void sendTemperature()
{
  float tempC;
  sensors.requestTemperatures();
  tempC = sensors.getTempC(sensor_black);  
  changeDomoticzSingleValue(TEMP_BLACK_IDX, tempC);
  tempC = sensors.getTempC(sensor_yellow);
  changeDomoticzSingleValue(TEMP_YELLOW_IDX, tempC);  
  tempC = sensors.getTempC(sensor_package);
  changeDomoticzSingleValue(TEMP_PACKAGE_IDX, tempC);    
}

void sendDHT11Value(){
  float temp = dht.getTemperature();
  float humid = dht.getHumidity();
  if( dht.getStatusString() == "OK" ){
    Sprint("[DHT11] Temp: ");
    Sprintln(temp);
    Sprint("[DHT11] Humid: ");
    Sprintln(humid);
  }else{
    Sprintln("[DHT11] status failed");
  }
  changeDomoticzDoubleValue(DHT11_IDX,temp,humid);
}

void getDeviceInfo(int var_idx){
  char tmp[5];
  payload_buf[0] = 0;
  itoa(var_idx,tmp,10);
  strcat(payload_buf,"&type=command&param=getuservariable&idx=");
  strcat(payload_buf,tmp);  
  Sprintln(payload_buf);
  httpRequest(payload_buf);
  switch_callback();
}

void changeGpioState(uint8_t gpio, uint8_t value){
    digitalWrite(gpio, value?HIGH:LOW);
}

void changeAllGpio(){
  changeGpioState(GPIO_SIREN, sirenState);
  changeGpioState(GPIO_LIGHT, lightState);
  changeGpioState(LED_GREEN, !greenState);  
  changeGpioState(LED_YELLOW, !yellowState);  
  changeGpioState(LED_RED, !redState);  
}

void checkDomoticzAllSwitchStatus(){
  getDeviceInfo(ALARM_VAR);
  wdt_reset();
  getDeviceInfo(SABOT_ALARM_VAR);
  wdt_reset();
  getDeviceInfo(SABOT_SIREN_VAR);
  wdt_reset();
  getDeviceInfo(SIREN_VAR);
  wdt_reset();
  getDeviceInfo(LIGHT_VAR);
  wdt_reset();
  getDeviceInfo(LED_GREEN_VAR);
  wdt_reset();
  getDeviceInfo(LED_YELLOW_VAR);
  wdt_reset();
  getDeviceInfo(LED_RED_VAR);
  wdt_reset();
  getDeviceInfo(ALARM_INDICATOR_VAR);
  wdt_reset();
  getDeviceInfo(ATTACK_INDICATOR_VAR);
  wdt_reset(); 
}

