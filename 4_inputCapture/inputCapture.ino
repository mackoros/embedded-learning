#include <Arduino.h>
#include <HardwareTimer.h>

// ---------- Пины ----------
#define TRIG_PIN  PA0
#define ECHO_PIN  PA1

// ---------- Тайминги ----------
#define TRIGGER_PULSE_US     12
#define TRIGGER_PERIOD_US    60
#define SERIAL_BAUD          115200

// ---------- Глобальные объекты и переменные ----------
HardwareTimer *timer;
uint32_t captureChannel = 2;   // заполним в setup() автоматически

//volatile uint32_t isrCount    = 0;
volatile uint32_t riseTime    = 0;
volatile uint32_t pulseWidth  = 0;
volatile bool     newMeas     = false;

// Колбэк вызывается HardwareTimer при каждом захвате
void onCapture() {
  //isrCount++;
  uint32_t captured = timer->getCaptureCompare(captureChannel);

  // Определяем фронт по уровню пина PA1
  if (GPIOA->IDR & GPIO_PIN_1) {
    riseTime = captured;
  } else {
    pulseWidth = captured - riseTime;
    newMeas = true;
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) delay(10);
  Serial.println("=== Input Capture (HardwareTimer API) ===");

  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  // Автоматическое определение таймера и канала для PA1
  TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(
      digitalPinToPinName(ECHO_PIN), PinMap_PWM);
  captureChannel = STM_PIN_CHANNEL(pinmap_function(
      digitalPinToPinName(ECHO_PIN), PinMap_PWM));

  Serial.print("Instance = 0x"); Serial.println((uint32_t)Instance, HEX);
  Serial.print("Channel  = ");   Serial.println(captureChannel);

  timer = new HardwareTimer(Instance);
  timer->setPrescaleFactor(timer->getTimerClkFreq() / 1000000);  // 1 тик = 1 мкс
  timer->setOverflow(0xFFFFFFFF);
  timer->setCount(0);

  timer->setMode(captureChannel, TIMER_INPUT_CAPTURE_BOTHEDGE, ECHO_PIN);
  timer->attachInterrupt(captureChannel, onCapture);
  timer->resume();

  Serial.print("Timer clk = ");
  Serial.print(timer->getTimerClkFreq());
  Serial.println(" Hz");
  Serial.println("Timer resumed. Waiting for edges...");
}

void loop() {
  static uint32_t lastTrig = 0;
  static uint32_t lastLog  = 0;
  uint32_t now = millis();

  // Триггер раз в 60 мс
  if (now - lastTrig >= TRIGGER_PERIOD_US) {
    lastTrig = now;
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(TRIGGER_PULSE_US);
    digitalWrite(TRIG_PIN, LOW);
  }

  // Новое измерение
  if (newMeas) {
    newMeas = false;
    uint32_t w = pulseWidth;
    if (w > 30000 || w < 116) {
      Serial.println("no echo / out of range");
    } else {
      Serial.print(w);
      Serial.print(" us -> ");
      Serial.print(w / 58.0f, 1);
      Serial.println(" cm");
    }
  }

  // Диагностика раз в секунду
  /*if (now - lastLog >= 1000) {
    lastLog = now;
    Serial.print("[diag] ISR count = ");
    Serial.print(isrCount);
    Serial.print(", level = ");
    Serial.println(GPIOA->IDR & GPIO_PIN_1 ? 1 : 0);
  }*/
}
