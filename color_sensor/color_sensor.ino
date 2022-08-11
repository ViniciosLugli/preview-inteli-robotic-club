#include <Adafruit_TCS34725.h>

#define BLUE_PIN 5
#define GREEN_PIN 18
#define RED_PIN 19

#define MIN_CLEAR_THRESHOLD 100

class Color {
public:
	uint16_t red;
	uint16_t green;
	uint16_t blue;
	uint16_t clear;

	Color(uint16_t red = 0, uint16_t green = 0, uint16_t blue = 0, uint16_t clear = 0) {
		this->red = red;
		this->green = green;
		this->blue = blue;
		this->clear = clear;
	}

	String getColor() {
		if (this->clear < MIN_CLEAR_THRESHOLD) {
			return "unknown";
		} else if (this->red > this->green && this->red > this->blue) {
			return "red";
		} else if (this->green > this->red && this->green > this->blue) {
			return "green";
		} else if (this->blue > this->red && this->blue > this->green) {
			return "blue";
		} else {
			return "unknown";
		}
	}
};

class LedRGB {
public:
	byte redPin;
	byte greenPin;
	byte bluePin;

	LedRGB(byte redPin, byte greenPin, byte bluePin) {
		this->redPin = redPin;
		this->greenPin = greenPin;
		this->bluePin = bluePin;

		pinMode(redPin, OUTPUT);
		pinMode(greenPin, OUTPUT);
		pinMode(bluePin, OUTPUT);

		digitalWrite(redPin, HIGH);
		digitalWrite(greenPin, HIGH);
		digitalWrite(bluePin, HIGH);
	}

	void setColor(String color) {
		if (color == "red") {
			digitalWrite(redPin, LOW);
			digitalWrite(greenPin, HIGH);
			digitalWrite(bluePin, HIGH);
		} else if (color == "green") {
			digitalWrite(redPin, HIGH);
			digitalWrite(greenPin, LOW);
			digitalWrite(bluePin, HIGH);
		} else if (color == "blue") {
			digitalWrite(redPin, HIGH);
			digitalWrite(greenPin, HIGH);
			digitalWrite(bluePin, LOW);
		} else {
			digitalWrite(redPin, HIGH);
			digitalWrite(greenPin, HIGH);
			digitalWrite(bluePin, HIGH);
		}
	}
};

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_1X);
LedRGB led = LedRGB(RED_PIN, GREEN_PIN, BLUE_PIN);

void setup() {
	Serial.begin(115200);
	if (tcs.begin()) {
		Serial.println("Found sensor");
	} else {
		Serial.println("No TCS34725 found ... check your connections");
		while (1)
			;
	}
}

void loop() {
	Color current_color = Color();

	tcs.getRawData(&current_color.red, &current_color.green, &current_color.blue, &current_color.clear);

	Serial.print("Current color: ");
	Serial.print(current_color.getColor());
	Serial.println("");
	Serial.print("Red: ");
	Serial.print(current_color.red);
	Serial.print(" Green: ");
	Serial.print(current_color.green);
	Serial.print(" Blue: ");
	Serial.print(current_color.blue);
	Serial.print(" Clear: ");
	Serial.println(current_color.clear);

	led.setColor(current_color.getColor());
}
