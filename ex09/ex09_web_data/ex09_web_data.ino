// ex09_touch_dashboard.ino
// 实时触摸传感器 Web 仪表盘

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "zzxwasxh";
const char* password = "sxhwazzx1912";

const int TOUCH_PIN = 4;

WebServer server(80);

// 主页面
String getHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>触摸仪表盘</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    #value { font-size: 48px; font-weight: bold; color: #2196F3; }
  </style>
</head>
<body>
  <h2>实时触摸传感器</h2>
  <p>当前数值：<span id="value">--</span></p>
  <script>
    function fetchData() {
      fetch('/data')
        .then(response => response.text())
        .then(data => {
          document.getElementById('value').innerText = data;
        })
        .catch(err => console.error('Error:', err));
    }
    setInterval(fetchData, 200); // 每200ms更新一次
    window.onload = fetchData;
  </script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", getHTML());
}

void handleData() {
  int val = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(val));
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected, IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}
