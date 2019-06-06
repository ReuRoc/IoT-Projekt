// based on example code 
// (c) Michael Schoeffler 2017, http://www.mschoeffler.de

#include "Wire.h" // This library allows you to communicate with I2C devices.
#include <Adafruit_NeoPixel.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define LED_PIN  2
#define LED_COUNT 3

// Data wire for temp sensor is connected to GPIO15
#define ONE_WIRE_BUS 4


const char* ssid = "FI_WLAN";
const char* password = "FI-Labor";

// MQTT Broker IP address
const char* mqtt_server = "10.43.0.77";

// MQTT channels
const char* mqtt_dir_chan = "device/2342/direction";
const char* mqtt_temp_chan = "device/2342/temperature";
const char* mqtt_led_state = "device/2342/state";


// global variables
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
//Change Temp Sensor Addresses
DeviceAddress sensor1 = { 0x28, 0xFF, 0x7A, 0xE3, 0xA0, 0x16, 0x4, 0x88 };
//DeviceAddress sensor1 = { 0x28, 0xFF, 0x7A, 0xE3, 0xA0, 0x16, 0x4, 0x88 };
//DeviceAddress sensor1 = { 0x28, 0xFF, 0x7A, 0xE3, 0xA0, 0x16, 0x4, 0x88 };
//DeviceAddress sensor1 = { 0x28, 0xFF, 0x7A, 0xE3, 0xA0, 0x16, 0x4, 0x88 };



long lastMsg = 0;
char msg[50];
OneWire ds(15);
const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
  
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data
int tangibledirection;

char tmp_str[7]; // temporary variable used in convert function

 
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
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}



void setup() {
  Serial.begin(112500);
  setup_wifi();
  initLed();
  initGyro();
  scanNetworks(); //scan for wifi networks
  connectToNetwork();
  sensors.begin();
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  
  //WiFi.disconnect(true);
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); //callback for MQTT events
  client.subscribe("/test");
 
}

void initLed() {
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
                 // Turn OFF all pixels ASAP
  strip.setBrightness(50);
  strip.setPixelColor(0,strip.Color(255,0,00));
  strip.show();
  strip.setPixelColor(1,strip.Color(0,0,255));
  strip.show();
}

void setup_wifi() {
  delay(500);
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
      client.subscribe("/test");
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
  String messageTemp;
  client.publish("/test","ich empfange");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
 /* if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }*/
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
 
void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
 
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
 // Serial.print("aX = "); Serial.print(convert_int16_to_str(accelerometer_x));
  //Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accelerometer_y));
  Serial.print(" Gyro Z-Axis = "); Serial.print(convert_int16_to_str(accelerometer_z));
  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  //Serial.print(" | tmp = "); Serial.print(temperature/340.00+36.53);
  //Serial.print(" | gX = "); Serial.print(convert_int16_to_str(gyro_x));
  //Serial.print(" | gY = "); Serial.print(convert_int16_to_str(gyro_y));
  //Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(gyro_z));
  Serial.println();

  //TODO: Lage bestimmen

  tangibledirection = 2; //default 
  
  if (accelerometer_z > 5000) {
    tangibledirection = 0;
  }
  if (accelerometer_z < -5000) {
    tangibledirection = 1;
  }

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
