#include "DHT.h"

#define DHTPIN PA0        // Пин данных DHT11 на Black Pill
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);     // USART1 по умолчанию на PA9 (TX) и PA10 (RX)
  dht.begin();
}

void loop() {
  delay(2000);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {
    Serial.println("Error reading DHT11");
    return;
  }
  Serial.print("T:"); Serial.print(t);
  Serial.print(",H:"); Serial.println(h);
}