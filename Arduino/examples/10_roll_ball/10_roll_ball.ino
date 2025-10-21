#include <Arduino_GFX_Library.h>
#include "FastIMU.h"
#include <Wire.h>
#include "esp_lcd_touch_axs5106l.h"

#define GFX_BL 23
#define IMU_ADDRESS 0x6B
#define ROTATION 0

#define Touch_I2C_SDA 18
#define Touch_I2C_SCL 19
#define Touch_RST     20
#define Touch_INT     21

// Display setup
Arduino_DataBus *bus = new Arduino_HWSPI(15 /* DC */, 14 /* CS */, 1 /* SCK */, 2 /* MOSI */);
Arduino_GFX *gfx = new Arduino_ST7789(
  bus, 22 /* RST */, 0 /* rotation */, false /* IPS */,
  172 /* width */, 320 /* height */,
  34 /*col_offset1*/, 0 /*uint8_t row_offset1*/,
  34 /*col_offset2*/, 0 /*row_offset2*/);

// IMU setup
QMI8658 IMU;
calData calib = { 0 };
AccelData accelData;

// Touch data
touch_data_t touch_data;

// Ball physics
float ballX = 86.0;
float ballY = 160.0;
float velocityX = 0.0;
float velocityY = 0.0;
const float ballRadius = 8.0;
const float friction = 0.98;
const float restitution = 0.85;
const float accelerationScale = 15.0;
const int displayWidth = 172;
const int displayHeight = 320;

int oldBallX = 86;
int oldBallY = 160;

// Game state
bool gameOver = false;
int score = 0;

// Stars
const int maxStars = 3;
struct Star {
  float x;
  float y;
  bool active;
  int size;
};
Star stars[maxStars];

// Enemies
const int maxEnemies = 10;
struct Enemy {
  float x;
  float y;
  float vx;
  float vy;
  float speed;
  int size;
  int oldX;
  int oldY;
  bool active;
};
Enemy enemies[maxEnemies];
const float baseEnemySpeed = 0.5;
int activeEnemyCount = 0;
unsigned long lastEnemySpawnTime = 0;
const unsigned long enemySpawnInterval = 10000; // 10 seconds

void lcd_reg_init(void) {
  static const uint8_t init_operations[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x11,
    END_WRITE,
    DELAY, 120,

    BEGIN_WRITE,
    WRITE_C8_D16, 0xDF, 0x98, 0x53,
    WRITE_C8_D8, 0xB2, 0x23,

    WRITE_COMMAND_8, 0xB7,
    WRITE_BYTES, 4,
    0x00, 0x47, 0x00, 0x6F,

    WRITE_COMMAND_8, 0xBB,
    WRITE_BYTES, 6,
    0x1C, 0x1A, 0x55, 0x73, 0x63, 0xF0,

    WRITE_C8_D16, 0xC0, 0x44, 0xA4,
    WRITE_C8_D8, 0xC1, 0x16,

    WRITE_COMMAND_8, 0xC3,
    WRITE_BYTES, 8,
    0x7D, 0x07, 0x14, 0x06, 0xCF, 0x71, 0x72, 0x77,

    WRITE_COMMAND_8, 0xC4,
    WRITE_BYTES, 12,
    0x00, 0x00, 0xA0, 0x79, 0x0B, 0x0A, 0x16, 0x79, 0x0B, 0x0A, 0x16, 0x82,

    WRITE_COMMAND_8, 0xC8,
    WRITE_BYTES, 32,
    0x3F, 0x32, 0x29, 0x29, 0x27, 0x2B, 0x27, 0x28, 0x28, 0x26, 0x25, 0x17, 0x12, 0x0D, 0x04, 0x00, 0x3F, 0x32, 0x29, 0x29, 0x27, 0x2B, 0x27, 0x28, 0x28, 0x26, 0x25, 0x17, 0x12, 0x0D, 0x04, 0x00,

    WRITE_COMMAND_8, 0xD0,
    WRITE_BYTES, 5,
    0x04, 0x06, 0x6B, 0x0F, 0x00,

    WRITE_C8_D16, 0xD7, 0x00, 0x30,
    WRITE_C8_D8, 0xE6, 0x14,
    WRITE_C8_D8, 0xDE, 0x01,

    WRITE_COMMAND_8, 0xB7,
    WRITE_BYTES, 5,
    0x03, 0x13, 0xEF, 0x35, 0x35,

    WRITE_COMMAND_8, 0xC1,
    WRITE_BYTES, 3,
    0x14, 0x15, 0xC0,

    WRITE_C8_D16, 0xC2, 0x06, 0x3A,
    WRITE_C8_D16, 0xC4, 0x72, 0x12,
    WRITE_C8_D8, 0xBE, 0x00,
    WRITE_C8_D8, 0xDE, 0x02,

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 3,
    0x00, 0x02, 0x00,

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 3,
    0x01, 0x02, 0x00,

    WRITE_C8_D8, 0xDE, 0x00,
    WRITE_C8_D8, 0x35, 0x00,
    WRITE_C8_D8, 0x3A, 0x05,

    WRITE_COMMAND_8, 0x2A,
    WRITE_BYTES, 4,
    0x00, 0x22, 0x00, 0xCD,

    WRITE_COMMAND_8, 0x2B,
    WRITE_BYTES, 4,
    0x00, 0x00, 0x01, 0x3F,

    WRITE_C8_D8, 0xDE, 0x02,

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 3,
    0x00, 0x02, 0x00,

    WRITE_C8_D8, 0xDE, 0x00,
    WRITE_C8_D8, 0x36, 0x00,
    WRITE_COMMAND_8, 0x21,
    END_WRITE,

    DELAY, 10,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x29,
    END_WRITE
  };
  bus->batchOperation(init_operations, sizeof(init_operations));
}

void spawnStar(int index) {
  stars[index].x = random(20, displayWidth - 20);
  stars[index].y = random(20, displayHeight - 20);
  stars[index].active = true;
  stars[index].size = 5;
}

void spawnEnemy(int index) {
  enemies[index].x = random(30, displayWidth - 30);
  enemies[index].y = random(30, displayHeight - 30);

  // Make sure enemy doesn't spawn too close to player
  float dx = enemies[index].x - ballX;
  float dy = enemies[index].y - ballY;
  float dist = sqrt(dx * dx + dy * dy);

  // If too close, spawn at edge
  if (dist < 50) {
    int edge = random(0, 4);
    switch(edge) {
      case 0: enemies[index].x = 30; enemies[index].y = random(30, displayHeight - 30); break;
      case 1: enemies[index].x = displayWidth - 30; enemies[index].y = random(30, displayHeight - 30); break;
      case 2: enemies[index].x = random(30, displayWidth - 30); enemies[index].y = 30; break;
      case 3: enemies[index].x = random(30, displayWidth - 30); enemies[index].y = displayHeight - 30; break;
    }
  }

  enemies[index].vx = 0;
  enemies[index].vy = 0;
  // Random speed between 100-200% of base speed (0.5 to 1.0)
  enemies[index].speed = baseEnemySpeed * (random(100, 201) / 100.0);
  enemies[index].size = 10;
  enemies[index].oldX = (int)enemies[index].x;
  enemies[index].oldY = (int)enemies[index].y;
  enemies[index].active = true;
  activeEnemyCount++;

  Serial.print("Spawned enemy #");
  Serial.print(index);
  Serial.print(" with speed: ");
  Serial.println(enemies[index].speed);
}

void drawStar(int x, int y, int size, uint16_t color) {
  // Draw a simple 4-pointed star
  gfx->drawLine(x, y - size, x, y + size, color);
  gfx->drawLine(x - size, y, x + size, y, color);
  gfx->drawLine(x - size + 1, y - size + 1, x + size - 1, y + size - 1, color);
  gfx->drawLine(x - size + 1, y + size - 1, x + size - 1, y - size + 1, color);
}

void initGame() {
  gameOver = false;
  score = 0;
  activeEnemyCount = 0;
  lastEnemySpawnTime = millis();

  // Reset ball
  ballX = 86.0;
  ballY = 160.0;
  velocityX = 0.0;
  velocityY = 0.0;
  oldBallX = 86;
  oldBallY = 160;

  // Initialize stars
  for (int i = 0; i < maxStars; i++) {
    spawnStar(i);
  }

  // Initialize all enemies as inactive
  for (int i = 0; i < maxEnemies; i++) {
    enemies[i].active = false;
  }

  // Spawn initial 2 enemies
  spawnEnemy(0);
  spawnEnemy(1);

  // Clear and redraw screen
  gfx->fillScreen(RGB565_BLACK);
  gfx->drawRect(0, 0, displayWidth, displayHeight, RGB565_WHITE);
}

void showGameOver() {
  gfx->fillScreen(RGB565_BLACK);

  gfx->setTextSize(2);
  gfx->setCursor(35, 100);
  gfx->setTextColor(RGB565_RED);
  gfx->println("GAME OVER");

  gfx->setTextSize(1);
  gfx->setCursor(50, 140);
  gfx->setTextColor(RGB565_WHITE);
  gfx->print("Score: ");
  gfx->println(score);

  // Draw restart button
  gfx->fillRect(36, 180, 100, 40, RGB565_GREEN);
  gfx->drawRect(36, 180, 100, 40, RGB565_WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(48, 192);
  gfx->setTextColor(RGB565_BLACK);
  gfx->println("RESTART");

  gfx->setTextSize(1);
  gfx->setCursor(10, 240);
  gfx->setTextColor(RGB565_CYAN);
  gfx->println("Tap RESTART to play");
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("Roll Ball Game");

  // Init Display
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }

  lcd_reg_init();
  gfx->setRotation(ROTATION);
  gfx->fillScreen(RGB565_BLACK);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  // Init IMU and Touch (both use Wire/I2C)
  Wire.begin(Touch_I2C_SDA, Touch_I2C_SCL);
  Wire.setClock(400000);

  // Init Touch
  bsp_touch_init(&Wire, Touch_RST, Touch_INT, ROTATION, displayWidth, displayHeight);
  Serial.println("Touch initialized");

  // Init IMU
  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
    gfx->setCursor(10, 10);
    gfx->setTextColor(RGB565_RED);
    gfx->setTextSize(2);
    gfx->println("IMU Init Failed!");
    while (true) {
      ;
    }
  }

  // Display instructions
  gfx->setCursor(10, 100);
  gfx->setTextColor(RGB565_CYAN);
  gfx->setTextSize(1);
  gfx->println("Collect stars!");
  gfx->setCursor(10, 120);
  gfx->println("Avoid red enemies!");
  gfx->setCursor(10, 140);
  gfx->println("Tilt to move");

  delay(3000);

  randomSeed(analogRead(0));
  initGame();
}

void loop() {
  if (gameOver) {
    // Check for touch on restart button
    // The touch library uses interrupts, so we need to call bsp_touch_read first
    bsp_touch_read();

    // Then get coordinates
    if (bsp_touch_get_coordinates(&touch_data)) {
      int touchX = touch_data.coords[0].x;
      int touchY = touch_data.coords[0].y;

      Serial.print("Touch detected! X: ");
      Serial.print(touchX);
      Serial.print(" Y: ");
      Serial.print(touchY);
      Serial.println();

      // Any touch on screen restarts the game
      Serial.println("Restarting game!");
      delay(300); // Debounce
      initGame();
    }
    delay(10); // Small delay to reduce CPU usage
    return;
  }

  // Update IMU data
  IMU.update();
  IMU.getAccel(&accelData);

  // Apply acceleration from tilting
  velocityX -= accelData.accelX * accelerationScale * 0.05;
  velocityY -= accelData.accelY * accelerationScale * 0.05;

  // Apply friction
  velocityX *= friction;
  velocityY *= friction;

  // Update ball position
  ballX += velocityX;
  ballY += velocityY;

  // Check for collisions with walls and bounce
  if (ballX - ballRadius < 0) {
    ballX = ballRadius;
    velocityX = -velocityX * restitution;
  } else if (ballX + ballRadius > displayWidth) {
    ballX = displayWidth - ballRadius;
    velocityX = -velocityX * restitution;
  }

  if (ballY - ballRadius < 0) {
    ballY = ballRadius;
    velocityY = -velocityY * restitution;
  } else if (ballY + ballRadius > displayHeight) {
    ballY = displayHeight - ballRadius;
    velocityY = -velocityY * restitution;
  }

  // Spawn new enemy every 10 seconds (up to max)
  if (activeEnemyCount < maxEnemies && millis() - lastEnemySpawnTime >= enemySpawnInterval) {
    // Find first inactive enemy slot
    for (int i = 0; i < maxEnemies; i++) {
      if (!enemies[i].active) {
        spawnEnemy(i);
        lastEnemySpawnTime = millis();
        break;
      }
    }
  }

  // Update and draw enemies
  for (int i = 0; i < maxEnemies; i++) {
    if (!enemies[i].active) continue;

    // Erase old enemy position
    gfx->fillRect(enemies[i].oldX - enemies[i].size/2 - 1,
                  enemies[i].oldY - enemies[i].size/2 - 1,
                  enemies[i].size + 2,
                  enemies[i].size + 2,
                  RGB565_BLACK);

    // Chase player
    float dx = ballX - enemies[i].x;
    float dy = ballY - enemies[i].y;
    float dist = sqrt(dx * dx + dy * dy);

    if (dist > 0) {
      enemies[i].vx = (dx / dist) * enemies[i].speed;
      enemies[i].vy = (dy / dist) * enemies[i].speed;
    }

    enemies[i].x += enemies[i].vx;
    enemies[i].y += enemies[i].vy;

    // Keep enemies in bounds
    enemies[i].x = constrain(enemies[i].x, enemies[i].size/2 + 2, displayWidth - enemies[i].size/2 - 2);
    enemies[i].y = constrain(enemies[i].y, enemies[i].size/2 + 2, displayHeight - enemies[i].size/2 - 2);

    // Draw new enemy position
    int ex = (int)enemies[i].x;
    int ey = (int)enemies[i].y;
    gfx->fillRect(ex - enemies[i].size/2, ey - enemies[i].size/2,
                  enemies[i].size, enemies[i].size, RGB565_RED);

    enemies[i].oldX = ex;
    enemies[i].oldY = ey;

    // Check collision with player
    float enemyDist = sqrt(dx * dx + dy * dy);
    if (enemyDist < ballRadius + enemies[i].size/2) {
      gameOver = true;
      showGameOver();
      return;
    }
  }

  // Check star collection
  for (int i = 0; i < maxStars; i++) {
    if (stars[i].active) {
      float dx = ballX - stars[i].x;
      float dy = ballY - stars[i].y;
      float dist = sqrt(dx * dx + dy * dy);

      if (dist < ballRadius + stars[i].size) {
        // Collect star
        drawStar((int)stars[i].x, (int)stars[i].y, stars[i].size + 2, RGB565_BLACK);
        score += 10;
        spawnStar(i);
      }
    }
  }

  // Erase old ball position
  gfx->fillCircle(oldBallX, oldBallY, (int)ballRadius + 1, RGB565_BLACK);

  // Redraw border sections if ball was near edge
  if (oldBallX - ballRadius <= 2) {
    gfx->drawFastVLine(0, max(0, oldBallY - (int)ballRadius - 2), (int)ballRadius * 2 + 4, RGB565_WHITE);
  }
  if (oldBallX + ballRadius >= displayWidth - 2) {
    gfx->drawFastVLine(displayWidth - 1, max(0, oldBallY - (int)ballRadius - 2), (int)ballRadius * 2 + 4, RGB565_WHITE);
  }
  if (oldBallY - ballRadius <= 2) {
    gfx->drawFastHLine(max(0, oldBallX - (int)ballRadius - 2), 0, (int)ballRadius * 2 + 4, RGB565_WHITE);
  }
  if (oldBallY + ballRadius >= displayHeight - 2) {
    gfx->drawFastHLine(max(0, oldBallX - (int)ballRadius - 2), displayHeight - 1, (int)ballRadius * 2 + 4, RGB565_WHITE);
  }

  // Draw stars
  for (int i = 0; i < maxStars; i++) {
    if (stars[i].active) {
      drawStar((int)stars[i].x, (int)stars[i].y, stars[i].size, RGB565_YELLOW);
    }
  }

  // Draw ball at new position
  int intBallX = (int)ballX;
  int intBallY = (int)ballY;

  float speed = sqrt(velocityX * velocityX + velocityY * velocityY);
  uint16_t ballColor;
  if (speed < 1.0) {
    ballColor = RGB565_GREEN;
  } else if (speed < 3.0) {
    ballColor = RGB565_YELLOW;
  } else if (speed < 5.0) {
    ballColor = RGB565_ORANGE;
  } else {
    ballColor = RGB565_RED;
  }

  gfx->fillCircle(intBallX, intBallY, (int)ballRadius, ballColor);
  gfx->fillCircle(intBallX - 2, intBallY - 2, 2, RGB565_WHITE);

  oldBallX = intBallX;
  oldBallY = intBallY;

  // Display score
  gfx->fillRect(5, 5, 50, 10, RGB565_BLACK);
  gfx->setCursor(5, 5);
  gfx->setTextSize(1);
  gfx->setTextColor(RGB565_CYAN);
  gfx->print("Score:");
  gfx->print(score);

  Serial.print("Score: ");
  Serial.print(score);
  Serial.print(" Ball: (");
  Serial.print(ballX);
  Serial.print(", ");
  Serial.print(ballY);
  Serial.println(")");

  delay(20);
}
