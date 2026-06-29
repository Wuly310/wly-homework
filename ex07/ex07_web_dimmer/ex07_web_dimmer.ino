// ex07_web_slider_dimmer.ino
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "zzxwasxh";
const char* password = "sxhwazzx1912";

const int LED_PIN = 2;
const int pwmChannel = 0;
const int freq = 5000;
const int resolution = 8;

WebServer server(80);

String getHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>调光器</title>
</head>
<body style="text-align:center;font-family:Arial;margin-top:50px;">
  <h2>ESP32 无极调光</h2>
  <input type="range" min="0" max="255" value="0" id="slider"
         oninput="sendValue(this.value)">
  <p>当前亮度: <span id="val">0</span></p >
  <script>
    function sendValue(val) {
      document.getElementById('val').innerText = val;
      fetch('/set?value=' + val);
    }
  </script>
</body>
</html>
)rawliteral";
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", getHTML());
}

void handleSet() {
  if (server.hasArg("value")) {
    int val = server.arg("value").toInt();
    val = constrain(val, 0, 255);
    ledcWrite(pwmChannel, val);
    Serial.print("Brightness: ");
    Serial.println(val);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(LED_PIN, pwmChannel);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected, IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();
}