#include "cdh.h"
#include "DHTStable.h"
DHTStable DHT;
#define DHT11_PIN D7
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "BLYNK_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "BLYNK_DEVICE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include "BlynkSimpleEsp8266.h"
// You should get Auth Token in the Blynk App.
char auth[] = "BLYNK_AUTH_TOKEN";
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YOUR_SSID"; //SSID of your wireless network
char pass[] = "YOUR_SSID_PASSWORD"; // password to your wireless network
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();
BlynkTimer timer;
int _curTempSet = _curTempSet;
bool _enabled = _enabled;
int _h1enabled = _h1enabled;
int _h2enabled = _h2enabled;
int _autostarttemp = _autostarttemp;
bool _autostartenable = _autostartenable;
bool _autostartactive = _autostartactive;
unsigned long int _autostarttime = _autostarttime;

int _doorbell = _doorbell;
bool _doorsensor = _doorsensor;
const int _doortrigpin =  _doortrigpin;



BLYNK_WRITE(V4) //h2onbutton
{
  _h2enabled = true;
  mySwitch.send("0110001101010101000100011011000");
  Blynk.virtualWrite(V2, 1); //set V2 h2onled HIGH
  Blynk.virtualWrite(V3, 0); //set V3 h2offled LOW
}
BLYNK_WRITE(V5) //h2offbutton
{
  _h2enabled = false;
  mySwitch.send("0110001101010101000001010000100");
  Blynk.virtualWrite(V2, 0); //set V2 h2onled LOW
  Blynk.virtualWrite(V3, 1); //set V2 h2offled HIGH
  _autostartactive = false;
}
BLYNK_WRITE(V8) //h1onbutton
{
  _h1enabled = true;
  mySwitch.send("11001001111101100001000110110000");
  Blynk.virtualWrite(V6, 1); //set h1onled HIGH
  Blynk.virtualWrite(V7, 0); //set h1offled LOW
}  
BLYNK_WRITE(V9) //h1offbutton
{
  _h1enabled = false;
  mySwitch.send("11001001111101100000010100001000");
  Blynk.virtualWrite(V6, 0); //set h1onled LOW
  Blynk.virtualWrite(V7, 1); //set h1offled HIGH
}
BLYNK_WRITE(V10) //curTempSet
{
  int pinValue = param.asInt();
  Blynk.virtualWrite(V10, _curTempSet); //set V10 to current temp
  Serial.println(pinValue);
}
BLYNK_WRITE(V11) //h1upbutton
{
  mySwitch.send("11001001111101100000100011110000");
  _curTempSet++;
  Blynk.virtualWrite(V10, _curTempSet); //set V10 to current temp
}
BLYNK_WRITE(V12) //h1downbutton
{
  mySwitch.send("0110001101010101000000101000100");
  _curTempSet--;
  Blynk.virtualWrite(V10, _curTempSet); //set V10 to current temp
}
BLYNK_WRITE(V13) //h2upbutton
{
  mySwitch.send("0110001101010101000010001111000");
  int pinValue = param.asInt();
  _curTempSet += 1;
  Blynk.virtualWrite(V10, _curTempSet); //set V10 to current temp
}
BLYNK_WRITE(V14) //h2downbutton
{
  mySwitch.send("0110001101010101000000101000100");
  int pinValue = param.asInt();
  _curTempSet -= 1;
  Blynk.virtualWrite(V10, _curTempSet); //set V10 to current temp
}

BLYNK_WRITE(V18) // Auto temperature settings
{
  int pinValue = param.asInt();
  Serial.println(pinValue);
  Blynk.virtualWrite(V18, pinValue); //
  _autostarttemp = pinValue;
}
BLYNK_WRITE(V19) // Auto temperature enable/disable
{
  int pinValue = param.asInt();
  Serial.println(pinValue);
  switch (pinValue)
  {
    case 0:
      Blynk.virtualWrite(V20, pinValue); //
      Blynk.virtualWrite(V21, !pinValue); //
      _autostartactive = false;
      _autostartenable = false;
      break;
    case 1:
      Blynk.virtualWrite(V20, pinValue); //
      Blynk.virtualWrite(V21, !pinValue); //
      _autostartenable = true;
      break;
  }
}

cdh::cdh(int heater, bool enabled)
{
  _heater = heater;
  _enabled = enabled;
}

void cdh::DODEFAULTWIDGETS()
{
  Blynk.virtualWrite(V10, _curTempSet); //Set V10 to current temp
  Blynk.virtualWrite(V17, 0); //set led on V16 to current 1 "Off"
}
BLYNK_WRITE(V15) // door alarm
{
  int pinValue = param.asInt();
  Serial.println(pinValue);
  if (pinValue == 1)
  {
    _doorsensor = true;
    Serial.println("Alarm Enabled");
    //Blynk.virtualWrite(V17, 0); // turn off alarm led
  }
  else if (pinValue == 0) {
    _doorsensor = false;

    Serial.println("Alarm Disabled");
  }
  if (!_doorsensor) {
    Blynk.virtualWrite(V17, pinValue); // turn off alarm led
  }
  Blynk.virtualWrite(V16, pinValue); //set led on V16 to current 1 "ON"
}

void cdh::DOSETUP()
{
  // SETUP DOOR SENSOR
  pinMode(_doortrigpin, OUTPUT);
  digitalWrite(_doortrigpin, LOW);
  // Initialize RCSwitch and select Pin which is used for sending the data
  mySwitch.enableTransmit(D8);
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  // Setup a function to be called every second
  //timer.setInterval(1000L, SENDSENSOR);
  //timer.setInterval(10000L, DoDefaultWidgetSettings);
}
void cdh::DOMAINLOOP()
{
    if (_autostartactive) {
    _timenow = millis();
    if (_timenow - _autostarttime > (_autostartshutdowntime * 60000)) {
      // timer reached for warm up shutdown heater
      Serial.print("Warm up complete, shutting down heater");
      mySwitch.send("0110001101010101000001010000100");
      Blynk.virtualWrite(V2, 0); //set V2 h2onled LOW
      Blynk.virtualWrite(V3, 1); //set V2 h2offled HIGH
      _autostartactive = false;
    }
  }
  //timer.setInterval(60000L, CHECKAUTOSTART);

  //checkDoor();
  CHECKDOOR();

  Blynk.run();
  timer.run();
}
void cdh::CHECKDOOR()
{
  if (_doorsensor == true)
  {
    if (digitalRead(_doortrigpin) == LOW)
    {
      Serial.println("Alarm Active");
      Blynk.virtualWrite(V17, 1); // alarm active turn on led
      mySwitch.send("111111000011110111110001"); //send signal to alarm
      delay(50);
    }
  }
}

void cdh::SENDSENSOR()
{
  float h = DHT.getHumidity();
  float t = DHT.getTemperature();
  Blynk.virtualWrite(V0, t); //set V0 to DHT11 Temp
  Blynk.virtualWrite(V1, h); //set V0 to DHT11 Humidity
}

void cdh::CHECKAUTOSTART()
{
  if (_autostartenable == true && _autostartactive == false)
  {
    Serial.print("Auto Start -- Checking Temperature....");
    Serial.println(DHT.getTemperature());
    if (DHT.getTemperature() <= _autostarttemp && _autostartactive == false)
    {
      //turn on heater 2
      _autostarttime = millis();
      _autostartactive = true;
      mySwitch.send("0110001101010101000100011011000");
      Blynk.virtualWrite(V2, 1); //set V2 h2onled HIGH
      Blynk.virtualWrite(V3, 0); //set V3 h2offled LOW
      Serial.println("Auto Start Trigger");
    }
  }
}
void cdh::READTEMPHUMSENSOR()
{
  // DISPLAY DATA
  DHT.read11(DHT11_PIN); 
  Serial.print(DHT.getHumidity(), 1);
  Serial.print(",");
  Serial.println(DHT.getTemperature(), 1);
  delay(2000);    
}
