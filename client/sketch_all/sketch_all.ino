// based on example code 
// by (c) Michael Schoeffler 2017, http://www.mschoeffler.de

#include "Wire.h" // This library allows you to communicate with I2C devices.

#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

// this client ID also selects the channel for the mqtt messages
#define CLIENT_ID "23"

//DeviceAddress sensorAddress = { 0x28, 0xFF, 0x2F, 0x47, 0xA1, 0x16, 0x5, 0x9D };//sensor 1
//DeviceAddress sensorAddress = { 0x28, 0xFF, 0x7A, 0xE3, 0xA0, 0x16, 0x4, 0x88 }; //sensor 2
DeviceAddress sensorAddress = { 0x28, 0xFF, 0x24, 0xE, 0xA1, 0x16, 0x3, 0x43 };//sensor 3
//DeviceAddress sensorAddress = { 0x28, 0xFF, 0x2B, 0x17, 0xA1, 0x16, 0x3, 0x87 };//sensor 4

const char* ssid = "FI_WLAN";
const char* password = "FI-Labor";


// MQTT Broker IP address
const char* mqtt_server = "10.43.0.77";

// MQTT channels
const char* mqtt_dir_chan = "devices/" CLIENT_ID "/direction";
const char* mqtt_temp_chan = "devices/" CLIENT_ID "/temperature";
const char* mqtt_state_chan = "devices/" CLIENT_ID "/state";


#define LED_PIN  2     // LED data wire is on pin G2
#define LED_COUNT 5    // there are 5 LEDs on most LED strands
#define ONE_WIRE_BUS 4 // Data wire for temp sensor is connected to GPIO4

// global variables
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WiFiClient espClient;
PubSubClient client(espClient);

// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
//Change Temp Sensor Addresses



const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data
long now=millis();
int tangibledirection = 0;
String statusColor = "";

void initLed() {
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();               
  strip.setBrightness(50);
  strip.clear();
  strip.setPixelColor(0,strip.Color(55,55,55));
  strip.setPixelColor(1,strip.Color(55,55,55));
  strip.setPixelColor(2,strip.Color(55,55,55));
  strip.setPixelColor(3,strip.Color(55,55,55));
  strip.show();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void initGyro() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client" CLIENT_ID)) {
      Serial.print("connected - ");
      Serial.println("ESP8266Client" CLIENT_ID);
      // Subscribe
      client.subscribe(mqtt_state_chan);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(500);
    }
  }
}
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTmp;
  
  for (int i = 0; i < length; i++) {
    messageTmp += (char)message[i];
  }
  Serial.print(messageTmp);
  Serial.print("+++++++++++");
  Serial.println();
  statusColor = messageTmp;
}

void readGyro() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
  
  //Lage bestimmen
  tangibledirection = 2; //default liegend
  if (accelerometer_z > 5000) {
    tangibledirection = 0;
  }
  if (accelerometer_z < -5000) {
    tangibledirection = 1;
  }

  // Ausgeben und an MQTT-Broker senden
  char dirString[8];
  dtostrf(tangibledirection, 1, 0, dirString);
  Serial.print(" Direction = ");
  Serial.print(dirString);
  Serial.println();
  client.publish(mqtt_dir_chan, dirString);
}

void readTemp(void){
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  char tempString[8];
  dtostrf(sensors.getTempC(sensorAddress), 1, 2, tempString);
  Serial.print("Temperatursensor (*C): ");
  Serial.println(tempString);
  client.publish(mqtt_temp_chan, tempString);
}

void updateLeds(){
  strip.clear();
  if (tangibledirection == 2) {
    strip.setPixelColor(1,strip.Color(0,0,255));  
  }
  if (tangibledirection == 0) {
    strip.setPixelColor(1,strip.Color(0,255,0)); 
  }
  if (tangibledirection == 1) {
     strip.setPixelColor(1,strip.Color(235,10,0)); 
  }
  uint32_t myColor = strip.Color(1,1,1);

  if(statusColor == "0"){
    myColor = strip.Color(0,255,0);
  }
  if(statusColor == "1"){
    myColor = strip.Color(255,83,0);
  }
  if(statusColor == "2"){
    myColor = strip.Color(0,0,255);
  }
  if(statusColor == "3"){
    myColor = strip.Color(255,0,0);
  }
  if(statusColor == "4"){
    myColor =  strip.Color(128,0,128);
  }
  if(statusColor == "5"){
    myColor =  strip.Color(255,255,255);
  }
  
  strip.setPixelColor(0,strip.Color(0,0,0));
  strip.setPixelColor(2,myColor);
  strip.setPixelColor(3,myColor);
  strip.setPixelColor(4,myColor);
  delay(1);      // see https://github.com/adafruit/Adafruit_NeoPixel/issues/139
  //portDISABLE_INTERRUPTS();
  strip.show();  // this does not really prevent flickering, but mitigates the problem a bit
                 // this is a fundamental problem in the adafruit neopixel lib
  //portENABLE_INTERRUPTS();
}


// ########################################
void setup() {
  Serial.begin(115200);
  setup_wifi();
  initLed();
  initGyro();
  sensors.begin();
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); //callback for MQTT events
  client.subscribe(mqtt_state_chan);
 
}

// ########################################
void loop() {
  if (client.connected()) {
    client.loop(); //process incoming MQTT messages
  }
  updateLeds();
  if(millis()>=now+1000){
    now = millis();
    if (!client.connected()) {
      reconnect();
    }
    readGyro();
    readTemp();
  }
  //delay(250);
}
