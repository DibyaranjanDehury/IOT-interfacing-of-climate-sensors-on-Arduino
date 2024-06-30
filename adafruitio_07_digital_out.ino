#include <SimpleDHT.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define WLAN_SSID       "V2065"
#define WLAN_PASS       "satya2005"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "satyajit1976"
#define AIO_KEY         "aio_UwBx987sQIfSiUSkdJ4tD2yDrV6V"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_BMP085 bmp;

int pinDHT11 = D5;
SimpleDHT11 dht11;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  bmp.begin();
  display.begin(SSD1306_I2C_ADDRESS, 1, 2); // SDA, SCL
  
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  connect();
}

void connect() {
  while (mqtt.connect() != 0) { delay(10000); }
}

void loop() {
  if (!mqtt.ping(3)) {
    if (!mqtt.connected()) connect();
  }

  byte temperature_data = 0;
  byte humidity_data = 0;
  float pressure_data = bmp.readPressure() / 100.0F; // convert to hPa

  if (dht11.read(pinDHT11, &temperature_data, &humidity_data, NULL) == SimpleDHTErrSuccess) {
    temperature.publish((int)temperature_data);
    humidity.publish((int)humidity_data);
    // Publish atmospheric pressure
    Adafruit_MQTT_Publish pressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pressure");
    pressure.publish(pressure_data);

    // Display on OLED screen
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Temp: ");
    display.print((int)temperature_data);
    display.println(" C");
    display.print("Humidity: ");
    display.print((int)humidity_data);
    display.println(" %");
    display.print("Pressure: ");
    display.print(pressure_data);
    display.println(" hPa");
    display.display();
    
    delay(5000);
  }
}
