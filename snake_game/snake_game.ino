#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define MAX_SCREEN_WIDTH 128
#define MAX_SCREEN_HEIGHT 64

#define UP_TOUCH_PIN T5
#define DOWN_TOUCH_PIN T4
#define LEFT_TOUCH_PIN T0
#define RIGHT_TOUCH_PIN T6
#define TOUCH_THRESHOLD 20

#define SNAKE_SPEED_MS 500
#define SNAKE_SPEED_MS_MIN 200
#define SNAKE_SPEED_STEP 100
#define SNAKE_START_LENGTH 3
#define SNAKE_MAX_LENGTH 50

#define DEFAULT_CELL_SIZE 5

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef enum { EMPTY, SNAKE, FOOD } CellType;
typedef struct {
	short x;
	short y;
} Cell;

class TouchButton {
public:
	TouchButton(int pin) { this->_pin = pin; }

	bool isPressed() { return touchRead(this->_pin) < TOUCH_THRESHOLD; };

private:
	int _pin;
};

class Collision {
public:
	static bool isCollision(Cell cell1, Cell cell2) {
		return cell1.x + DEFAULT_CELL_SIZE > cell2.x && cell1.x < cell2.x + DEFAULT_CELL_SIZE && cell1.y + DEFAULT_CELL_SIZE > cell2.y &&
			   cell1.y < cell2.y + DEFAULT_CELL_SIZE;
	}
};

class Input {
public:
	bool isUpPressed() { return this->_upButton.isPressed(); }
	bool isDownPressed() { return this->_downButton.isPressed(); }
	bool isLeftPressed() { return this->_leftButton.isPressed(); }
	bool isRightPressed() { return this->_rightButton.isPressed(); }

private:
	TouchButton _upButton = TouchButton(UP_TOUCH_PIN);
	TouchButton _downButton = TouchButton(DOWN_TOUCH_PIN);
	TouchButton _leftButton = TouchButton(LEFT_TOUCH_PIN);
	TouchButton _rightButton = TouchButton(RIGHT_TOUCH_PIN);
};

class Screen {
public:
	Screen() {
		this->_screen.begin(SH1106_SWITCHCAPVCC, 0x3C);
		this->_screen.clearDisplay();
		this->_screen.setTextSize(1);
		this->_screen.setTextColor(WHITE);
		this->_screen.setCursor(0, 0);
		this->drawTextCenter("Snake Game", MAX_SCREEN_WIDTH / 2, MAX_SCREEN_HEIGHT / 2);
		this->_screen.display();
		delay(1000);
		this->_screen.clearDisplay();
	}

	void draw(Cell cell, CellType type) {
		if (cell.x < 0 || cell.x >= MAX_SCREEN_WIDTH || cell.y < 0 || cell.y >= MAX_SCREEN_HEIGHT) {
			return;
		}
		this->_screen.fillRect(cell.x, cell.y, DEFAULT_CELL_SIZE, DEFAULT_CELL_SIZE, WHITE);
	}

	void drawText(const String text, int x, int y, int size = 1) {
		this->_screen.setTextSize(size);
		this->_screen.setCursor(x, y);
		this->_screen.print(text);
	}

	void drawTextCenter(const String& text, int x, int y, int size = 1) {
		int16_t x1, y1;
		uint16_t w, h;
		this->_screen.getTextBounds(text, x, y, &x1, &y1, &w, &h);
		this->_screen.setCursor(x - w / 2, y);
		this->_screen.print(text);
	}

	void display() { this->_screen.display(); }

	void clear() { this->_screen.clearDisplay(); }

private:
	Adafruit_SH1106 _screen = Adafruit_SH1106(SDA, SCL);
};

class Snake {
public:
	Snake() {
		for (byte i = 1; i <= SNAKE_START_LENGTH; i++) {
			Cell cell = Cell{this->_head.x - (i * DEFAULT_CELL_SIZE), this->_head.y};
			Serial.println("Appending cell: " + String(cell.x) + "," + String(cell.y));
			this->appendCell(cell);
		}
	}

	Cell getHead() { return this->_head; }

	void move() {
		Cell oldHead = this->_head;
		if (this->_direction == UP) {
			this->_head.y -= DEFAULT_CELL_SIZE;
		} else if (this->_direction == DOWN) {
			this->_head.y += DEFAULT_CELL_SIZE;
		} else if (this->_direction == LEFT) {
			this->_head.x -= DEFAULT_CELL_SIZE;
		} else if (this->_direction == RIGHT) {
			this->_head.x += DEFAULT_CELL_SIZE;
		}

		// Move cells
		Cell* buffer = new Cell[this->_length];

		for (byte i = 0; i < this->_length; i++) {
			buffer[i] = this->_body[i];
		}

		buffer[0] = oldHead;
		for (byte i = 1; i < this->_length; i++) {
			buffer[i] = this->_body[i - 1];
		}

		for (byte i = 0; i < this->_length; i++) {
			this->_body[i] = buffer[i];
		}

		delete[] buffer;
	}

	void grow() { this->appendCell(Cell{-DEFAULT_CELL_SIZE, -DEFAULT_CELL_SIZE}); }

	void setDirection(Direction direction) { this->_direction = direction; }
	Direction getDirection() { return this->_direction; }

	bool isCollidingWithSelf() {
		for (int i = 0; i < this->_length; i++) {
			;
			if (Collision::isCollision(this->_head, this->_body[i])) {
				return true;
			}
		}
		return false;
	}

	bool isCollidingWithWall() {
		return this->_head.x < 0 || this->_head.x >= MAX_SCREEN_WIDTH || this->_head.y < 0 || this->_head.y >= MAX_SCREEN_HEIGHT;
	}

	bool isHeadColliding(Cell cell) { return Collision::isCollision(this->_head, cell); }

	bool isCollidingWithCell(Cell cell) {
		for (int i = 0; i < this->_length; i++) {
			if (Collision::isCollision(this->_body[i], cell)) {
				return true;
			}
		}
		return Collision::isCollision(this->_head, cell);
	}

	void draw(Screen pencil) {
		for (int i = 0; i < this->_length; i++) {
			pencil.draw(this->_body[i], SNAKE);
		}
		pencil.draw(this->_head, SNAKE);
	}

	void reset() {
		this->_length = SNAKE_START_LENGTH;
		this->_head = Cell{MAX_SCREEN_WIDTH / 2, MAX_SCREEN_HEIGHT / 2};
		this->_direction = RIGHT;
	}

private:
	void appendCell(Cell cell) {
		if (this->_length < SNAKE_MAX_LENGTH) {
			this->_body[this->_length++] = cell;
		}
	}

	Direction _direction = RIGHT;
	byte _length = 0;
	Cell _head = {MAX_SCREEN_WIDTH / 2, MAX_SCREEN_HEIGHT / 2};
	Cell _body[SNAKE_MAX_LENGTH] = {};
};

class Food {
public:
	byte getX() { return this->_position.x; }
	byte getY() { return this->_position.y; }
	Cell getPosition() { return this->_position; }

	void generate(Snake snake) {
		Serial.println("Generating food...");
		Cell position;
		position.x = random(DEFAULT_CELL_SIZE, MAX_SCREEN_WIDTH - DEFAULT_CELL_SIZE);
		position.y = random(DEFAULT_CELL_SIZE + 5, MAX_SCREEN_HEIGHT - DEFAULT_CELL_SIZE);
		this->_position = position;
		Serial.println("Food generated!");
	}

	void draw(Screen screen) { screen.draw(this->_position, FOOD); }

private:
	Cell _position;
};

class Timer {
public:
	bool update() {
		if (millis() - this->_lastUpdate > this->_currentStepsMs) {
			this->_lastUpdate = millis();
			return true;
		}
		return false;
	}

	void increaseSpeed() {
		this->_currentStepsMs -= SNAKE_SPEED_STEP;
		if (this->_currentStepsMs < SNAKE_SPEED_MS_MIN) {
			this->_currentStepsMs = SNAKE_SPEED_MS_MIN;
		}
	}

private:
	unsigned long _lastUpdate = millis();
	unsigned long _currentStepsMs = SNAKE_SPEED_MS;
};

Screen screen;
Snake snake;
Food food;
Input input;
Timer timer;

class Game {
public:
	static byte score;
	static Direction nextDirection;

	static void init() {
		Game::score = 0;
		screen = Screen();
		snake = Snake();
		food = Food();
		input = Input();
		timer = Timer();
		food.generate(snake);
	}
	static void handleInput() {
		if (input.isUpPressed() && snake.getDirection() != DOWN) {
			Serial.println("Up pressed");
			Game::nextDirection = UP;
		}
		if (input.isDownPressed() && snake.getDirection() != UP) {
			Serial.println("Down pressed");
			Game::nextDirection = DOWN;
		}
		if (input.isLeftPressed() && snake.getDirection() != RIGHT) {
			Serial.println("Left pressed");
			Game::nextDirection = LEFT;
		}
		if (input.isRightPressed() && snake.getDirection() != LEFT) {
			Serial.println("Right pressed");
			Game::nextDirection = RIGHT;
		}
	}
	static void update() {
		snake.setDirection(Game::nextDirection);
		snake.move();
		if (snake.isCollidingWithWall()) {
			Serial.println("Snake collided with wall!");
			Game::gameOver();
		} else if (snake.isCollidingWithSelf()) {
			Serial.println("Snake collided with self!");
			Game::gameOver();
		} else if (snake.isHeadColliding(food.getPosition())) {
			Game::score++;
			snake.grow();
			timer.increaseSpeed();
			food.generate(snake);
		}
	}
	static void render() {
		screen.clear();

		food.draw(screen);
		snake.draw(screen);
		screen.drawTextCenter("Score: " + String(Game::score), MAX_SCREEN_WIDTH / 2, 5);

		screen.display();
	}

	static void gameOver() {
		Serial.println("Game over");
		screen.clear();
		screen.drawTextCenter("GAME OVER", MAX_SCREEN_WIDTH / 2, MAX_SCREEN_HEIGHT / 2);
		screen.display();
		while (true) {
			if (input.isUpPressed()) {
				snake.reset();
				Game::init();
				break;
			}
		}
	}
};

byte Game::score = 0;
Direction Game::nextDirection = RIGHT;

void setup() {
	Serial.begin(115200);
	Serial.println("Initializing...");
	Game::init();
	Serial.println("Initialized!");
}

void loop() {
	Game::handleInput();
	if (timer.update()) {
		Game::update();
		Game::render();
	}
}