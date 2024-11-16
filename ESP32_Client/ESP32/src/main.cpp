#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

#define USE_SERIAL Serial

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
			webSocket.sendTXT("Connected");
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] Server: %s\n", payload);
			break;
        default:
            break;
	}

}

void setup() {
	USE_SERIAL.begin(9600);

	WiFi.begin("ABB", "123123123");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        USE_SERIAL.printf(".");
    }
    USE_SERIAL.printf("\nWiFi Connected!");
	webSocket.begin("192.168.22.59", 81, "/");
	webSocket.onEvent(webSocketEvent);

}

unsigned long lastMessageTime = 0;
const unsigned long messageInterval = 5000;

void loop() {
	webSocket.loop();

    unsigned long currentMillis = millis();
    if (currentMillis - lastMessageTime >= messageInterval) {
        lastMessageTime = currentMillis;
        if (webSocket.isConnected()) {
            webSocket.sendTXT("Hello, Server!");
        }
    }
}