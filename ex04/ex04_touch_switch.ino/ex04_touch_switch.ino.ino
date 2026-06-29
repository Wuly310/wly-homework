// ex04_touch_self_lock.ino
// 触摸自锁开关（边缘检测 + 软件防抖）

#define TOUCH_PIN 4      // T0 对应 GPIO4
#define LED_PIN   2      // 板载 LED

int threshold = 40;      // 触摸阈值（根据串口打印调整）
bool ledState = false;   // 当前 LED 状态
bool lastTouch = false;  // 上一次触摸状态
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // 防抖延迟（毫秒）

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  int touchValue = touchRead(TOUCH_PIN);
  bool touched = (touchValue < threshold);  // 触摸时为 true

  // 边缘检测：当前触摸且上次未触摸
  if (touched && !lastTouch) {
    // 防抖处理
    if (millis() - lastDebounceTime > debounceDelay) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      Serial.print("Toggle LED to: ");
      Serial.println(ledState ? "ON" : "OFF");
      lastDebounceTime = millis();
    }
  }
  lastTouch = touched;
  delay(10);  // 简单轮询间隔
}