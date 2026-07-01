// ex08_security_alarm_ajax.ino
#include <WiFi.h>
#include <WebServer.h>

// AP模式配置
const char* ssid = "ESP32_Security";
const char* password = "12345678";

const int LED_PIN = 2;
const int TOUCH_PIN = 4;
const int THRESHOLD = 40;

WebServer server(80);

bool armed = false;
bool alarmActive = false;
unsigned long lastAlarmToggle = 0;
const unsigned long alarmInterval = 200;

// 生成HTML（包含精美UI）
String getHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>智能安防报警器</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }
    
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      background: linear-gradient(135deg, #0f0c29, #302b63, #24243e);
      padding: 20px;
    }
    
    .container {
      background: rgba(255, 255, 255, 0.1);
      backdrop-filter: blur(20px);
      border-radius: 30px;
      padding: 50px 40px;
      max-width: 450px;
      width: 100%;
      box-shadow: 0 25px 50px rgba(0, 0, 0, 0.5);
      border: 1px solid rgba(255, 255, 255, 0.18);
      transition: all 0.3s ease;
    }
    
    .header {
      text-align: center;
      margin-bottom: 35px;
    }
    
    .header .icon {
      font-size: 60px;
      margin-bottom: 10px;
      display: block;
    }
    
    .header h1 {
      color: #ffffff;
      font-size: 28px;
      font-weight: 600;
      letter-spacing: 1px;
      text-shadow: 0 2px 10px rgba(0, 0, 0, 0.3);
    }
    
    .header p {
      color: rgba(255, 255, 255, 0.6);
      font-size: 14px;
      margin-top: 5px;
    }
    
    .status-container {
      background: rgba(0, 0, 0, 0.3);
      border-radius: 20px;
      padding: 25px;
      margin-bottom: 30px;
      text-align: center;
      border: 1px solid rgba(255, 255, 255, 0.08);
    }
    
    .status-label {
      color: rgba(255, 255, 255, 0.7);
      font-size: 14px;
      text-transform: uppercase;
      letter-spacing: 2px;
      margin-bottom: 8px;
    }
    
    .status-value {
      font-size: 32px;
      font-weight: 700;
      padding: 8px 0;
      transition: all 0.5s ease;
    }
    
    .status-armed {
      color: #ff6b6b;
      text-shadow: 0 0 20px rgba(255, 107, 107, 0.3);
    }
    
    .status-disarmed {
      color: #51cf66;
      text-shadow: 0 0 20px rgba(81, 207, 102, 0.3);
    }
    
    .status-alarm {
      color: #ff0000;
      text-shadow: 0 0 40px rgba(255, 0, 0, 0.6);
      animation: blink 0.5s ease-in-out infinite;
    }
    
    @keyframes blink {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.3; }
    }
    
    .indicator {
      display: inline-block;
      width: 14px;
      height: 14px;
      border-radius: 50%;
      margin-right: 10px;
      vertical-align: middle;
      transition: all 0.3s ease;
    }
    
    .indicator-armed {
      background: #ff6b6b;
      box-shadow: 0 0 15px rgba(255, 107, 107, 0.5);
    }
    
    .indicator-disarmed {
      background: #51cf66;
      box-shadow: 0 0 15px rgba(81, 207, 102, 0.5);
    }
    
    .indicator-alarm {
      background: #ff0000;
      box-shadow: 0 0 25px rgba(255, 0, 0, 0.8);
      animation: blink 0.5s ease-in-out infinite;
    }
    
    .button-group {
      display: flex;
      gap: 15px;
      margin-bottom: 15px;
    }
    
    .btn {
      flex: 1;
      padding: 16px 20px;
      border: none;
      border-radius: 16px;
      font-size: 18px;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.3s ease;
      text-transform: uppercase;
      letter-spacing: 1px;
      position: relative;
      overflow: hidden;
    }
    
    .btn:active {
      transform: scale(0.95);
    }
    
    .btn-arm {
      background: linear-gradient(135deg, #ff6b6b, #ee5a24);
      color: white;
      box-shadow: 0 8px 20px rgba(255, 107, 107, 0.3);
    }
    
    .btn-arm:hover {
      transform: translateY(-2px);
      box-shadow: 0 12px 30px rgba(255, 107, 107, 0.4);
    }
    
    .btn-arm:active {
      transform: translateY(0px);
    }
    
    .btn-disarm {
      background: linear-gradient(135deg, #51cf66, #2d9e4a);
      color: white;
      box-shadow: 0 8px 20px rgba(81, 207, 102, 0.3);
    }
    
    .btn-disarm:hover {
      transform: translateY(-2px);
      box-shadow: 0 12px 30px rgba(81, 207, 102, 0.4);
    }
    
    .btn-disarm:active {
      transform: translateY(0px);
    }
    
    .btn:disabled {
      opacity: 0.5;
      cursor: not-allowed;
      transform: none !important;
    }
    
    .info-bar {
      display: flex;
      justify-content: space-between;
      margin-top: 25px;
      padding-top: 20px;
      border-top: 1px solid rgba(255, 255, 255, 0.08);
    }
    
    .info-item {
      text-align: center;
      flex: 1;
    }
    
    .info-item .label {
      color: rgba(255, 255, 255, 0.4);
      font-size: 11px;
      text-transform: uppercase;
      letter-spacing: 1px;
    }
    
    .info-item .value {
      color: rgba(255, 255, 255, 0.8);
      font-size: 18px;
      font-weight: 600;
      margin-top: 4px;
    }
    
    .alarm-badge {
      display: none;
      background: rgba(255, 0, 0, 0.2);
      border: 1px solid rgba(255, 0, 0, 0.3);
      border-radius: 12px;
      padding: 12px;
      margin-top: 15px;
      text-align: center;
      color: #ff6b6b;
      font-weight: 600;
      font-size: 14px;
      animation: blink 0.5s ease-in-out infinite;
    }
    
    .alarm-badge.active {
      display: block;
    }
    
    .ap-info {
      background: rgba(0, 255, 0, 0.1);
      border: 1px solid rgba(0, 255, 0, 0.2);
      border-radius: 12px;
      padding: 10px;
      margin-top: 15px;
      text-align: center;
      color: rgba(255, 255, 255, 0.7);
      font-size: 12px;
    }
    
    .ap-info strong {
      color: #51cf66;
    }
    
    @media (max-width: 480px) {
      .container {
        padding: 30px 20px;
      }
      
      .header h1 {
        font-size: 22px;
      }
      
      .status-value {
        font-size: 26px;
      }
      
      .btn {
        font-size: 15px;
        padding: 14px 16px;
      }
    }
  </style>
  <script>
    let currentStatus = '';
    
    function getStatus() {
      fetch('/status')
        .then(res => res.text())
        .then(data => {
          updateUI(data);
        })
        .catch(err => {
          console.error('Error fetching status:', err);
        });
    }
    
    function updateUI(data) {
      const statusEl = document.getElementById('status');
      const indicatorEl = document.getElementById('indicator');
      const alarmBadge = document.getElementById('alarmBadge');
      const armBtn = document.getElementById('armBtn');
      const disarmBtn = document.getElementById('disarmBtn');
      
      currentStatus = data;
      
      // 更新状态文本和样式
      if (data.includes('报警')) {
        statusEl.textContent = '🚨 报警中!';
        statusEl.className = 'status-value status-alarm';
        indicatorEl.className = 'indicator indicator-alarm';
        alarmBadge.classList.add('active');
        armBtn.disabled = true;
        disarmBtn.disabled = false;
      } else if (data.includes('布防')) {
        statusEl.textContent = '🔒 已布防';
        statusEl.className = 'status-value status-armed';
        indicatorEl.className = 'indicator indicator-armed';
        alarmBadge.classList.remove('active');
        armBtn.disabled = true;
        disarmBtn.disabled = false;
      } else {
        statusEl.textContent = '🔓 已撤防';
        statusEl.className = 'status-value status-disarmed';
        indicatorEl.className = 'indicator indicator-disarmed';
        alarmBadge.classList.remove('active');
        armBtn.disabled = false;
        disarmBtn.disabled = true;
      }
    }
    
    function setArm(arm) {
      const armBtn = document.getElementById('armBtn');
      const disarmBtn = document.getElementById('disarmBtn');
      
      armBtn.disabled = true;
      disarmBtn.disabled = true;
      
      fetch('/set?arm=' + arm)
        .then(() => getStatus())
        .finally(() => {
          // 重新启用按钮取决于状态
          setTimeout(() => {
            getStatus();
          }, 100);
        });
    }
    
    // 每秒自动更新状态
    setInterval(getStatus, 1000);
    
    // 页面加载时获取状态
    window.onload = function() {
      getStatus();
    };
  </script>
</head>
<body>
  <div class="container">
    <div class="header">
      <span class="icon">🛡️</span>
      <h1>智能安防系统</h1>
      <p>ESP32 触摸报警器</p>
    </div>
    
    <div class="status-container">
      <div class="status-label">系统状态</div>
      <div>
        <span class="indicator" id="indicator"></span>
        <span class="status-value status-disarmed" id="status">加载中...</span>
      </div>
    </div>
    
    <div class="button-group">
      <button class="btn btn-arm" id="armBtn" onclick="setArm(1)">🔐 布防</button>
      <button class="btn btn-disarm" id="disarmBtn" onclick="setArm(0)">🔓 撤防</button>
    </div>
    
    <div class="alarm-badge" id="alarmBadge">
      ⚠️ 警告：检测到非法入侵！
    </div>
    
    <div class="ap-info">
      🌐 连接WiFi: <strong>ESP32_Security</strong> | 密码: <strong>12345678</strong><br>
      访问: <strong>http://192.168.4.1</strong>
    </div>
    
    <div class="info-bar">
      <div class="info-item">
        <div class="label">传感器</div>
        <div class="value">触摸</div>
      </div>
      <div class="info-item">
        <div class="label">阈值</div>
        <div class="value">40</div>
      </div>
      <div class="info-item">
        <div class="label">状态</div>
        <div class="value">● 在线</div>
      </div>
    </div>
  </div>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", getHTML());
}

void handleStatus() {
  String state = "";
  if (alarmActive) {
    state = "报警中!";
  } else if (armed) {
    state = "布防";
  } else {
    state = "撤防";
  }
  server.send(200, "text/plain", state);
}

void handleSet() {
  if (server.hasArg("arm")) {
    int val = server.arg("arm").toInt();
    armed = (val == 1);
    if (!armed) {
      alarmActive = false;
      digitalWrite(LED_PIN, LOW);
    }
    Serial.println(armed ? "Armed" : "Disarmed");
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 设置为AP模式
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  // 等待AP启动
  delay(1000);
  
  // 获取AP的IP地址
  IPAddress IP = WiFi.softAPIP();
  Serial.println();
  Serial.println("========================================");
  Serial.println("ESP32 Security System - AP Mode");
  Serial.println("========================================");
  Serial.print("AP SSID: ");
  Serial.println(ssid);
  Serial.print("AP Password: ");
  Serial.println(password);
  Serial.print("AP IP Address: ");
  Serial.println(IP);
  Serial.println("========================================");
  Serial.println("请在浏览器访问: http://192.168.4.1");
  Serial.println("========================================");
  
  // 设置服务器路由
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/set", handleSet);
  server.begin();
  Serial.println("HTTP Server Started!");
}

void loop() {
  server.handleClient();

  static bool lastTouched = false;
  int touchVal = touchRead(TOUCH_PIN);
  bool touched = (touchVal < THRESHOLD);

  if (armed && touched && !lastTouched) {
    alarmActive = true;
    Serial.println("⚠️ ALARM TRIGGERED!");
  }
  lastTouched = touched;

  if (alarmActive) {
    if (millis() - lastAlarmToggle > alarmInterval) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      lastAlarmToggle = millis();
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}
