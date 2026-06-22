/*
 * 实验3: 使用millis()函数控制LED产生SOS闪烁信号
 * SOS信号: 三次短闪(200ms) + 三次长闪(600ms) + 三次短闪(200ms)
 * 每次SOS播放完毕后有较长的熄灭停顿(2000ms)
 */

const int ledPin = 2;  
// 定义时间参数 (单位: 毫秒)
const unsigned long SHORT_FLASH = 200;   // 短闪持续时间
const unsigned long LONG_FLASH = 600;    // 长闪持续时间
const unsigned long PAUSE_SHORT = 200;   // 短闪之间的停顿
const unsigned long PAUSE_LONG = 400;    // 长闪之间的停顿
const unsigned long SOS_PAUSE = 2000;    // SOS循环结束后的长停顿

// 定义SOS信号序列
// 0 = 熄灭, 1 = 短闪, 2 = 长闪
const int sosSequence[] = {
  1, 0,  // S: 短闪-熄灭
  1, 0,  // S: 短闪-熄灭
  1, 0,  // S: 短闪-熄灭
  2, 0,  // O: 长闪-熄灭
  2, 0,  // O: 长闪-熄灭
  2, 0,  // O: 长闪-熄灭
  1, 0,  // S: 短闪-熄灭
  1, 0,  // S: 短闪-熄灭
  1, 0   // S: 短闪-熄灭
};

const int sequenceLength = sizeof(sosSequence) / sizeof(sosSequence[0]);

unsigned long previousMillis = 0;
int currentStep = 0;          // 当前序列步骤
bool isSosPause = false;      // 是否处于SOS循环后的长停顿
unsigned long sosPauseStart = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  // 处理SOS循环结束后的长停顿
  if (isSosPause) {
    if (currentMillis - sosPauseStart >= SOS_PAUSE) {
      isSosPause = false;
      currentStep = 0;  // 重置到序列开始
    }
    return;  // 停顿期间不执行其他操作
  }

  // 检查当前步骤是否需要执行
  unsigned long stepDuration = 0;
  int signalType = sosSequence[currentStep];
  
  // 计算当前步骤的持续时间
  if (signalType == 1) {  // 短闪
    stepDuration = SHORT_FLASH;
  } else if (signalType == 2) {  // 长闪
    stepDuration = LONG_FLASH;
  } else {  // 熄灭 (0)
    // 根据前一步骤决定熄灭时间
    if (currentStep > 0) {
      int prevSignal = sosSequence[currentStep - 1];
      if (prevSignal == 1) {
        stepDuration = PAUSE_SHORT;  // 短闪后的停顿
      } else if (prevSignal == 2) {
        stepDuration = PAUSE_LONG;   // 长闪后的停顿
      } else {
        stepDuration = PAUSE_SHORT;
      }
    } else {
      stepDuration = PAUSE_SHORT;
    }
  }

  // 检查是否到达步骤切换时间
  if (currentMillis - previousMillis >= stepDuration) {
    previousMillis = currentMillis;

    // 点亮或熄灭LED
    if (signalType == 0) {
      digitalWrite(ledPin, LOW);
    } else {
      digitalWrite(ledPin, HIGH);
    }

    // 移动到下一步
    currentStep++;
    
    // 检查是否完成整个SOS序列
    if (currentStep >= sequenceLength) {
      isSosPause = true;
      sosPauseStart = currentMillis;
      digitalWrite(ledPin, LOW);  // 确保LED熄灭
    }
  }
}