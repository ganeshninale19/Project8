#include <WiFi.h>               //to connect wifi
#include <HTTPClient.h>         // to make http request
#include <OneWire.h>            //to read temperature from DS18B20
#include <DallasTemperature.h>  //to read temperature from DS18B20
#include <esp_wifi.h>           //to change mac address of esp32
#include <ArduinoJson.h>        //to parse the JSON responce from server

// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 4
#define proximity 5
#define LDR 35

const char* ssid = "acts";  //wifi name
const char* password = "";  //wifi password

//Your Domain name with URL path or IP address with path
String serverName = "http://192.168.76.192:80/";

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

uint8_t newMACAddress[] = { 0xf4, 0x96, 0x34, 0x9d, 0xe0, 0xf9 };

String Request;

//devices and their laststatus
String devices[5] = { "geyser", "light", "AC", "fan" };

bool laststatus[5] = { 0, 0, 0, 0, 0 };

int count = 0;
//switch inputs pins
int inputs[4] = { 18, 19, 21, 22 };

//appliance output pins
int outputs[4] = { 12, 13, 14, 27 };

//to store the sensor value
bool fanstatus, status;
int brightness, temp, value;

//to store the message from server
StaticJsonDocument<200> payload;

//timer
hw_timer_t* My_timer = NULL;

///timer interrupt handler/////////
void IRAM_ATTR onTimer() {
  fanstatus = 0;  //automatically clear fanstatus if proximity interrupt don't come for 1 sec
}
/////////////////

///////External interrupt handler////////////
void IRAM_ATTR isr() {
  fanstatus = 1;
  timerRestart(My_timer);  //restarting the timer so that it shoudn't clear fanstatus for 1 sec
  timerAlarmEnable(My_timer);
}
/////////////////////

void setup(void) {
  /////////////connecting to wifi//////////////
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);  //changing the MAC address of the ESP32
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  ////////////////////////////////////////////

  //////////////Enabling timer interrupt////////
  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 1000000, true);
  timerAlarmEnable(My_timer);  //Just Enable
  /////////////////////////////

  ///////////////making  GPIO input/output//////////
  for (int i = 0; i < 4; i++) {
    pinMode(inputs[i], INPUT);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(outputs[i], OUTPUT);
  }
  ////////////////////////////

  pinMode(proximity, INPUT_PULLUP);
  attachInterrupt(proximity, isr, FALLING);
  // Start serial communication for debugging purposes
  Serial.begin(9600);
  // Start up the library
  sensors.begin();
}

void loop(void) {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures();

  if (WiFi.status() == WL_CONNECTED) {
    //fanstatus = digitalRead(proximity);
    brightness = analogRead(LDR);
    brightness = map(brightness, 0, 4095, 0, 1000);
    temp = sensors.getTempCByIndex(0);

    status = digitalRead(inputs[count]);

    if (laststatus[count] != status) {

      laststatus[count] = status;

      if (devices[count] == "AC")
        Update(devices[count], status, temp, brightness);
      else if (devices[count] == "geyser")
        Update(devices[count], status, -1, brightness);
      else
        Update(devices[count], status, -1, -1);
    }

    fetch(devices[count]);

    count = (count + 1) % 4;

    digitalWrite(outputs[payload[0].as<int>() - 1], payload[2].as<int>());
  }

  delay(1000);
}

void Update(String appliance, int status, int value, int power) {

  HTTPClient http;
  Request = serverName + "update?appliance=" + appliance + "&status=" + String(status) + "&value=" + String(value) + "&power=" + String(power);

  Serial.println(Request);

  http.begin(Request.c_str());

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void fetch(String appliance) {
  HTTPClient http;
  Request = serverName + "fetch?appliance=" + appliance;

  Serial.println(Request);

  http.begin(Request.c_str());

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {

    deserializeJson(payload, http.getStream());
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    //Serial.println(http.getString());
    Serial.println(payload[3].as<String>());
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
