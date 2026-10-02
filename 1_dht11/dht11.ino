#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Ошибка чтения DHT11!");
    return;
  }

  Serial.print("Температура: ");
  Serial.print(t);
  Serial.print(" °C, Влажность: ");
  Serial.print(h);
  Serial.println(" %");
}
