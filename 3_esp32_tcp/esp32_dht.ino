#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("ESP32 DHT11 test");
}

void loop() {
  delay(2000);

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Error reading DHT11");
    return;
  }

  Serial.print("T:");
  Serial.print(t);
  Serial.print(",H:");
  Serial.println(h);
}
