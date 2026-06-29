// ex05_touch_breath_gear.ino
#define TOUCH_PIN 4
#define LED_PIN   2

int threshold = 40;
int gear = 1;
bool lastTouch = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 20;

// 各档位对应的步进延迟（ms）
const int gearDelay[3] = {10, 5, 2};

// PWM通道（ESP32有16个通道，这里用0）
const int pwmChannel = 0;
const int freq = 5000;
const int resolution = 8;  // 0~255

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  // 配置PWM
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(LED_PIN, pwmChannel);
}

void loop() {
  // 触摸切换档位（边缘检测+防抖）
  int touchValue = touchRead(TOUCH_PIN);
  bool touched = (touchValue < threshold);
  if (touched && !lastTouch) {
    if (millis() - lastDebounceTime > debounceDelay) {
      gear = (gear % 3) + 1;
      Serial.print("Gear: ");
      Serial.println(gear);
      lastDebounceTime = millis();
    }
  }
  lastTouch = touched;

  // 呼吸灯效果（根据档位调节速度）
  int stepDelay = gearDelay[gear - 1];
  // 渐亮
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(pwmChannel, duty);
    delay(stepDelay);
  }
  // 渐暗
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(pwmChannel, duty);
    delay(stepDelay);
  }
}