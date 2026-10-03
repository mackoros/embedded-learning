// запасной вариант без таймерного захвата.
// Фронты Echo ловим внешним прерыванием (EXTI), время берём из micros().
#include <Arduino.h>

#define TRIG_PIN PA0
#define ECHO_PIN PA1

volatile uint32_t tRise = 0;      // момент переднего фронта Echo, мкс
volatile uint32_t width = 0;      // длительность импульса Echo, мкс
volatile uint32_t edges = 0;      // сколько фронтов видел пин
volatile bool     fresh = false;  // появилось новое измерение

// Вызывается на каждом изменении уровня Echo 
void onEcho() {
  edges++;
  if (digitalRead(ECHO_PIN)) {
    tRise = micros();             // Echo стал 1: запоминаем время
  } else {
    width = micros() - tRise;     // Echo стал 0: считаем длительность
    fresh = true;
  }
}

void setup() {
  Serial.begin(115200);
  // ждём USB-терминал не дольше 3 с, чтобы не зависнуть, если его не открыли
  uint32_t t0 = millis();
  while (!Serial && millis() - t0 < 3000) { delay(10); }

  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  // подтяжка к земле - без датчика пин читается как 0
  pinMode(ECHO_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), onEcho, CHANGE);

  Serial.println("HC-SR04 / EXTI test");
}

void loop() {
  static uint32_t lastTrig = 0;
  static uint32_t lastLog  = 0;
  uint32_t now = millis();

  // Запуск измерения раз в 60 мс
  if (now - lastTrig >= 60) {
    lastTrig = now;
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(12);
    digitalWrite(TRIG_PIN, LOW);
  }

  // Есть новое измерение - печатаем
  if (fresh) {
    fresh = false;
    uint32_t w = width;
    if (w > 30000) {
      Serial.println("no echo (timeout)");
    } else {
      Serial.print(w);
      Serial.print(" us -> ");
      Serial.print(w / 58.0f, 1);
      Serial.println(" cm");
    }
  }

  // Раз в секунду уровень на пине и счётчик фронтов
  if (now - lastLog >= 1000) {
    lastLog = now;
    Serial.print("[diag] level=");
    Serial.print(digitalRead(ECHO_PIN));
    Serial.print(" edges=");
    Serial.println(edges);
  }
}
