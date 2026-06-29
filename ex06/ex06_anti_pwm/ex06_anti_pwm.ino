// ex06_dual_pwm_flash.ino
#define LED_A_PIN 4
#define LED_B_PIN 5

// 两个PWM通道
const int channelA = 0;
const int channelB = 1;
const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  ledcSetup(channelA, freq, resolution);
  ledcAttachPin(LED_A_PIN, channelA);
  ledcSetup(channelB, freq, resolution);
  ledcAttachPin(LED_B_PIN, channelB);
}

void loop() {
  // A 渐亮，B 渐暗
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(channelA, duty);
    ledcWrite(channelB, 255 - duty);
    delay(5);
  }
  // A 渐暗，B 渐亮（反向）
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(channelA, duty);
    ledcWrite(channelB, 255 - duty);
    delay(6);
  }
}