#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define PADDLE_HEIGHT 12
#define PADDLE_WIDTH 2
#define BALL_SIZE 3

#define JOY1_Y A0  // Joystick pro hráče 1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int paddle1Y, paddle2Y;
float ballX, ballY;
float ballSpeedX, ballSpeedY;
float speedMultiplier = 1.05;
float maxSpeed = 6.0;
float aiSpeed = 2.5;  // Rychlost pohybu AI pálky

void setup() {
  pinMode(JOY1_Y, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  resetGame();
}

void loop() {
  // Ovládání hráče 1 (joystick)
  int joy1 = analogRead(JOY1_Y);
  paddle1Y = map(joy1, 0, 1023, 0, SCREEN_HEIGHT - PADDLE_HEIGHT);

  // AI pálka – pohyb směrem k míčku, ale ne příliš trhaně
  if (ballSpeedX > 0) {  
    if (paddle2Y + PADDLE_HEIGHT / 2 < ballY - 3) {
      paddle2Y += min(aiSpeed, abs(ballSpeedX));
    } else if (paddle2Y + PADDLE_HEIGHT / 2 > ballY + 3) {
      paddle2Y -= min(aiSpeed, abs(ballSpeedX));
    }
  }

  // Pohyb míčku
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Kolize s horní a dolní hranou
  if (ballY <= 0 || ballY >= SCREEN_HEIGHT - BALL_SIZE) {
    ballSpeedY = -ballSpeedY;
    ballY = constrain(ballY, 1, SCREEN_HEIGHT - BALL_SIZE - 1);
  }

  // Kolize s hráčovou pálkou
  if (ballX <= PADDLE_WIDTH && ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_HEIGHT) {
    ballSpeedX = abs(ballSpeedX);
    ballSpeedY = (ballY - (paddle1Y + PADDLE_HEIGHT / 2)) * 0.3;
    increaseSpeed();
  }

  // Kolize s AI pálkou
  if (ballX >= SCREEN_WIDTH - PADDLE_WIDTH - BALL_SIZE && ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_HEIGHT) {
    ballSpeedX = -abs(ballSpeedX);
    ballSpeedY = (ballY - (paddle2Y + PADDLE_HEIGHT / 2)) * 0.3;
    increaseSpeed();
  }

  // Pokud míček opustí obrazovku
  if (ballX < -BALL_SIZE || ballX > SCREEN_WIDTH + BALL_SIZE) {
    resetGame();
  }

  // Kreslení
  display.clearDisplay();
  display.fillRect(0, paddle1Y, PADDLE_WIDTH, PADDLE_HEIGHT, SSD1306_WHITE);
  display.fillRect(SCREEN_WIDTH - PADDLE_WIDTH, paddle2Y, PADDLE_WIDTH, PADDLE_HEIGHT, SSD1306_WHITE);
  display.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, SSD1306_WHITE);
  display.display();
  delay(15);
}

void resetGame() {
  paddle1Y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
  paddle2Y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
  ballX = SCREEN_WIDTH / 2;
  ballY = random(10, SCREEN_HEIGHT - 10);
  ballSpeedX = (random(0, 2) == 0) ? 2 : -2;
  ballSpeedY = (random(0, 2) == 0) ? 1 : -1;
}

void increaseSpeed() {
  ballSpeedX *= speedMultiplier;
  ballSpeedY *= speedMultiplier;
  ballSpeedX = constrain(ballSpeedX, -maxSpeed, maxSpeed);
  ballSpeedY = constrain(ballSpeedY, -maxSpeed, maxSpeed);
}
