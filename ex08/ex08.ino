// ex08_security_alarm_ajax.ino
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "zzxwasxh";
const char* password = "sxhwazzx1912";

const int LED_PIN = 2;
const int TOUCH_PIN = 4;
const int THRESHOLD = 25;

WebServer server(80);

bool armed = false;
bool alarmActive = false;
unsigned long lastAlarmToggle = 0;
const unsigned long alarmInterval = 200;

// 生成HTML（使用AJAX获取状态）
String getHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>安防报警器</title>
  <script>
    function getStatus() {
      fetch('/status')
        .then(res => res.text())
        .then(data => {
          document.getElementById('status').innerText = data;
        });
    }
    function setArm(arm) {
      fetch('/set?arm=' + arm)
        .then(() => getStatus());
    }
    setInterval(getStatus, 1000); // 每秒更新状态
    window.onload = getStatus;
  </script>
</head>
<body style="text-align:center;font-family:Arial;margin-top:50px;">
  <h2>ESP32 安防系统</h2>
  <p>当前状态: <b id="status">--</b></p >
  <button onclick="setArm(1)">布防</button>
  <button onclick="setArm(0)">撤防</button>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", getHTML());
}

void handleStatus() {
  String state = armed ? "布防" : "撤防";
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

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected, IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();

  static bool lastTouched = false;
  int touchVal = touchRead(TOUCH_PIN);
  bool touched = (touchVal < THRESHOLD);

  if (armed && touched && !lastTouched) {
    alarmActive = true;
    Serial.println("ALARM TRIGGERED!");
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