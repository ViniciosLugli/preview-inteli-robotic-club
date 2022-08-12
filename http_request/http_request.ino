#include <ESP32Servo.h>

#include "ESPAsyncWebServer.h"
#include "WiFi.h"

Servo servo;

const char* ssid = "esp32-api";
const char* password = "1234567890";

AsyncWebServer server(3000);

void setup() {
	Serial.begin(115200);
	servo.attach(13);

	WiFi.softAP(ssid, password);
	Serial.print("AP IP address: " + WiFi.softAPIP().toString());

	server.on("/servo", HTTP_POST, [](AsyncWebServerRequest* request) {
		if (servo.read() == 0) {
			servo.write(180);
		} else {
			servo.write(0);
		}
		request->send_P(200, "text/plain", "success");
	});

	server.begin();
}

void loop() {}