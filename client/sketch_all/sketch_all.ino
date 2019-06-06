// based on example code 
// (c) Michael Schoeffler 2017, http://www.mschoeffler.de

#include "Wire.h" // This library allows you to communicate with I2C devices.

#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN  2
#define LED_COUNT 4
// Data wire for temp sensor is connected to GPIO15
#define ONE_WIRE_BUS 4

const char* ssid = "FI_WLAN";
const char* password = "FI-Labor";

// MQTT Broker IP address
const char* mqtt_server = "10.43.0.77";

// MQTT channels
const char* mqtt_dir_chan = "devices/0/direction";
const char* mqtt_temp_chan = "devices/0/temperature";
const char* mqtt_state_chan = "devices/0/color";

//DeviceAddress sensor1 = { 0x28, 0xFF, 0x2F, 0x47, 0xA1, 0x16, 0x5, 0x9D };//sensor 1
DeviceAddress sensor1 = { 0x28, 0xFF, 0x7A, 0xE3, 0xA0, 0x16, 0x4, 0x88 }; //sensor 2
//DeviceAddress sensor1 = { 0x28, 0xFF, 0x24, 0xE, 0xA1, 0x16, 0x3, 0x43 };//sensor 3
//DeviceAddress sensor1 = { 0x28, 0xFF, 0x2B, 0x17, 0xA1, 0x16, 0x3, 0x87 };//sensor 4


// global variables
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WiFiClient espClient;
PubSubClient client(espClient);

// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
//Change Temp Sensor Addresses

long lastMsg = 0;
char msg[50];

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data
int tangibledirection;



String translateEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}


// helper function
char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  char tmp_str[7]; // temporary variable used in convert function
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}



void setup() {
  Serial.begin(112500);
  setup_wifi();
  initLed();
  initGyro();
  //scanNetworks(); //scan for wifi networks
  sensors.begin();
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); //callback for MQTT events
  client.subscribe(mqtt_state_chan);
 
}

void initLed() {
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
                 // Turn OFF all pixels ASAP
  strip.setBrightness(50);
  //strip.setPixelColor(0,strip.Color(55,55,55));
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
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
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
  uint32_t myColor = strip.Color(10,10,10);

  if(messageTmp == "green"){
    myColor = strip.Color(0,255,0);
  }
  if(messageTmp == "blue"){
    myColor = strip.Color(0,0,255);
  }
  if(messageTmp == "red"){
    myColor = strip.Color(255,0,0);
  }
  if(messageTmp == "orange"){
    myColor = strip.Color(255,83,0);
  }
  if(messageTmp == "purple"){
    myColor =  strip.Color(128,0,128);
  }
  strip.setPixelColor(2,myColor);
  strip.setPixelColor(3,myColor);
  strip.show();

}

void scanNetworks() {
 
  int numberOfNetworks = WiFi.scanNetworks();
 
  Serial.print("Number of networks found: ");
  Serial.println(numberOfNetworks);
 
  for (int i = 0; i < numberOfNetworks; i++) {
 
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));
 
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));
 
    Serial.print("MAC address: ");
    Serial.println(WiFi.BSSIDstr(i));
 
    Serial.print("Encryption type: ");
    String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
    Serial.println(encryptionTypeDescription);
    Serial.println("-----------------------");
 
  }
}
 




void readGyro()
{
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
  
  // print out data
  //Serial.print(" Acc Z-Axis = "); Serial.print(convert_int16_to_str(accelerometer_z));
  //Serial.println();

  //TODO: Lage bestimmen

  tangibledirection = 2; //default liegend
  
  if (accelerometer_z > 5000) {
    tangibledirection = 0;
    //strip.setPixelColor(0,strip.Color(0,255,0));  
    strip.setPixelColor(1,strip.Color(0,255,0));  
  }
  if (accelerometer_z < -5000) {
    tangibledirection = 1;
    //strip.setPixelColor(0,strip.Color(255,0,0));  
    strip.setPixelColor(1,strip.Color(255,0,0)); 
  }
  if (tangibledirection == 2) {
    //strip.setPixelColor(0,strip.Color(0,0,255));  
    strip.setPixelColor(1,strip.Color(0,0,255));  
  }
  strip.show();

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
  //Serial.println("DONE");
  Serial.print("Sensor 1(*C): ");
  Serial.println(sensors.getTempC(sensor1)); 
  char tempString[8];
  dtostrf(sensors.getTempC(sensor1), 1, 2, tempString);
 Serial.print(" Temp = ");
 Serial.print(tempString);
 Serial.println();
 client.publish(mqtt_temp_chan, tempString);

}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  readGyro();
  readTemp();
  long now = millis();
  delay(1000);
}
