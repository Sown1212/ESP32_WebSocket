#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

const char* ssid = "ESP32-AP";
const char* password = "12345678"; 

IPAddress local_IP(192, 168, 1, 1);      
IPAddress gateway(192, 168, 1, 1);      
IPAddress subnet(255, 255, 255, 0);   

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const int ledPin = 2;  
bool ledState = false;

const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Điều khiển LED</title>
</head>
<body>
    <h1>ESP32 WebSocket</h1>
    <button onclick="toggleLed()">Toggle LED</button>
    <p>Trạng thái LED: <span id='ledState'>OFF</span></p>
  <script>
    let socket = new WebSocket('ws://' + window.location.host + '/ws');
    socket.onmessage = (event) => {
      document.getElementById('ledState').innerText = event.data; 
    };
    function toggleLed() { 
      socket.send('toggle'); 
    }
  </script>
</body>
</html>
)rawliteral";

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("Client connected");
  } 
  else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected");
  }
  else if (type == WS_EVT_DATA) {
    String message = (char*)data;
    if (message == "toggle") {
      ledState = !ledState;               
      digitalWrite(ledPin, ledState);     
      client->text(ledState ? "ON" : "OFF"); 
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  setCpuFrequencyMhz(80);
  Serial.print("\nTần số CPU hiện tại: ");
  Serial.print(getCpuFrequencyMhz());
  Serial.println(" MHz");

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  Serial.println("Wi-Fi Access Point đã khởi động!");
  Serial.print("Địa chỉ IP: ");
  Serial.println(WiFi.softAPIP());

  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", htmlContent);
  });
  server.begin();
}

void loop() {
  delay(1000);
}
