#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// ESP8266 I2C pins (D1 = SCL, D2 = SDA)
#define OLED_SDA D2
#define OLED_SCL D1

// Initialize the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Initial player position
int playerX = SCREEN_WIDTH / 2;
int playerY = SCREEN_HEIGHT - 10;
int playerSize = 10;

// Obstacles
struct Obstacle {
  int x;
  int y;
  int width;
};

#define MAX_OBSTACLES 5
Obstacle obstacles[MAX_OBSTACLES];
int obstacleSpeed = 2;  

// Score and level
int score = 0;
int level = 1;
bool gameOver = false;

// Button pins (ESP8266)
const int leftButtonPin = D5;
const int rightButtonPin = D6;

void initObstacles() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    obstacles[i].x = random(0, SCREEN_WIDTH - 10);
    obstacles[i].y = random(-100, -10);
    obstacles[i].width = 10;
  }
}

void drawObstacles() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    display.fillRect(obstacles[i].x, obstacles[i].y, obstacles[i].width, 5, SSD1306_WHITE);
  }
}

void updateObstacles() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    obstacles[i].y += obstacleSpeed;

    if (obstacles[i].y > SCREEN_HEIGHT) {
      obstacles[i].y = random(-100, -10);
      obstacles[i].x = random(0, SCREEN_WIDTH - 10);
      score++;
    }
  }
}

bool checkCollision() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacles[i].y + 5 > playerY && obstacles[i].y < playerY + playerSize &&
        obstacles[i].x < playerX + playerSize && obstacles[i].x + obstacles[i].width > playerX) {
      return true;
    }
  }
  return false;
}

void drawPlayer() {
  display.fillRect(playerX, playerY, playerSize, playerSize, SSD1306_WHITE);
}

void increaseLevel() {
  level++;
  obstacleSpeed++;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.print("Level ");
  display.println(level);
  display.display();
  delay(3000);
}

void setup() {
  Serial.begin(115200);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED display not found!"));
    while (true);
  }

  display.clearDisplay();
  initObstacles();

  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
}

void loop() {
  if (!gameOver) {
    display.clearDisplay();
    drawObstacles();
    drawPlayer();
    display.display();
    updateObstacles();

    if (checkCollision()) {
      gameOver = true;
    }

    if (digitalRead(leftButtonPin) == LOW && playerX > 0) {
      playerX -= 5;
    }
    if (digitalRead(rightButtonPin) == LOW && playerX < SCREEN_WIDTH - playerSize) {
      playerX += 5;
    }

    if (score > 0 && score % 50 == 0) {
      increaseLevel();
      score++;
    }

    delay(50);
  } else {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.println("Game Over!");
    display.setCursor(0, 30);
    display.print("Score: ");
    display.println(score);
    display.display();
    delay(5000);

    gameOver = false;
    score = 0;
    level = 1;
    obstacleSpeed = 2;
    playerX = SCREEN_WIDTH / 2;
    playerY = SCREEN_HEIGHT - 10;
    initObstacles();
  }
}
