// ESP32 LED引脚定义
const int ledPin = 2;
unsigned long prevTime = 0;
const unsigned long interval = 1000; // 1Hz周期1000ms
bool ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currTime = millis();
  // 判断是否达到1秒间隔
  if (currTime - prevTime >= interval) {
    prevTime = currTime;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    Serial.print("LED状态：");
    Serial.println(ledState ? "亮" : "灭");
  }
}