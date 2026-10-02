#include <WiFi.h>
#include "DHT.h"
#include "esp_wifi.h"

const char* AP_SSID = "ESP32-Sensor";
const char* AP_PASS = "12345678";      // минимум 8 символов
const uint16_t PORT = 5000;

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(PORT);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_AP);

  //6 канал - raspberry pi никак не подключается при использовании 1 канала
  WiFi.softAP(AP_SSID, AP_PASS, 6, false, 4);
  esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20);

  Serial.print("new stuff done");

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());     // будет 192.168.4.1

  server.begin();
  Serial.println("TCP server started");
}

void loop() {
  if (!client || !client.connected()) {
    client = server.available();
    if (client) Serial.println("RPi connected");
  }

  if (client && client.connected()) {
    static unsigned long last = 0;
    if (millis() - last > 2000) {
      last = millis();
      float t = dht.readTemperature();
      float h = dht.readHumidity();
      if (!isnan(t) && !isnan(h)) {
        char buf[32];
        snprintf(buf, sizeof(buf), "T:%.1f,H:%.1f\n", t, h);
        client.print(buf);
        Serial.print("Sent: "); Serial.print(buf);
      }
    }
  }
}
