#include <U8g2lib.h>
#include <Wire.h>

/* =====================================================
   ================== OLED SETUP =======================
   ===================================================== */
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
// If SSD1306, use below instead
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

/* =====================================================
   ================== INPUT SYSTEM =====================
   ===================================================== */
#define BTN_ADC 0

enum Button { NONE, LEFT, UP, DOWN, RIGHT, RESET };

Button readButton() {
  int v = analogRead(BTN_ADC);

  if (v >= 0    && v <= 200)  return LEFT;
  if (v >= 600  && v <= 800)  return UP;
  if (v >= 1300 && v <= 1600) return DOWN;
  if (v >= 2000 && v <= 2400) return RIGHT;
  if (v >= 3200 && v <= 3500) return RESET;

  return NONE;
}

/* ================== BATTLESHIP GLOBALS ================== */

#define BS_SIZE 4

uint8_t bsShips[BS_SIZE][BS_SIZE];   // 1 = ship
uint8_t bsShots[BS_SIZE][BS_SIZE];   // 0=unknown, 1=miss, 2=hit

int bsCursorX = 0;
int bsCursorY = 0;

bool bsGameOver = false;

/* =====================================================
   ================== GLOBAL CONFIG ====================
   ===================================================== */
#define CELL 4
#define OFFSET 2
#define PLAY_W (128 - 4)
#define PLAY_H (64  - 4)

/* =====================================================
   ================== DOOM GLOBALS ====================
   ===================================================== */
#define MAX_ENEMIES 3

float doomX, doomY;
float doomA;

bool gunFlash = false;
unsigned long gunTime = 0;

struct Enemy {
  float x;
  float y;
  bool alive;
};

Enemy enemies[MAX_ENEMIES];

unsigned long sw5PressTime = 0;
bool sw5Held = false;

/* =====================================================
   ================== ANIMATION ========================
   ===================================================== */
namespace Animation {

  void flashBorder(int times = 2) {
    for (int i = 0; i < times; i++) {
      u8g2.clearBuffer();
      u8g2.drawFrame(OFFSET, OFFSET, PLAY_W, PLAY_H);
      u8g2.sendBuffer();
      delay(80);
      u8g2.clearBuffer();
      u8g2.sendBuffer();
      delay(80);
    }
  }

  void drawSnakeSegment(int x, int y, bool head) {
    if (head) u8g2.drawBox(x, y, CELL, CELL);
    else      u8g2.drawFrame(x, y, CELL, CELL);
  }
}

/* =====================================================
   ================== SYSTEM STATE =====================
   ===================================================== */
enum AppState { BOOT, MENU, SNAKE, PONG, DODGE, DOOM, BATTLESHIP };


AppState state = BOOT;

/* =====================================================
   ================== BOOT ANIMATION ===================
   ===================================================== */
void bootAnimation() {
  u8g2.setFont(u8g2_font_5x7_mf);

  // Pulse dot
  for (int r = 2; r <= 10; r += 2) {
    u8g2.clearBuffer();
    u8g2.drawDisc(64, 32, r);
    u8g2.sendBuffer();
    delay(60);
  }
  for (int r = 10; r >= 2; r -= 2) {
    u8g2.clearBuffer();
    u8g2.drawDisc(64, 32, r);
    u8g2.sendBuffer();
    delay(60);
  }

  // Draw border
  u8g2.clearBuffer();
  u8g2.drawFrame(OFFSET, OFFSET, PLAY_W, PLAY_H);
  u8g2.drawStr(36, 34, "MINI GAMES");
  u8g2.sendBuffer();
  delay(900);
}

/* =====================================================
   ================== MENU =============================
   ===================================================== */
const char* menuItems[] = { "SNAKE", "PONG", "DODGE", "DOOM" };

int menuIndex = 0;

void menuUpdate() {
  Button b = readButton();

  if (b == UP && menuIndex > 0) menuIndex--;
  if (b == DOWN && menuIndex < 3) menuIndex++;

  if (b == RESET) {
    if (menuIndex == 0) state = SNAKE;
    if (menuIndex == 1) state = PONG;
    if (menuIndex == 2) state = DODGE;
    if (menuIndex == 3) {
      doomReset();
      state = DOOM;
    }
    delay(300); // debounce
  }
}


void menuDraw() {
  u8g2.setFont(u8g2_font_5x7_mf);
  u8g2.drawStr(36, 12, "MINI GAMES");
  for (int i = 0; i < 4; i++) {
    u8g2.drawStr(32, 28 + i * 10,
      menuIndex == i ? "> " : "  ");
    u8g2.drawStr(44, 28 + i * 10, menuItems[i]);
  }
}

/* =====================================================
   ================== GAME: SNAKE ======================
   ===================================================== */
#define MAX_LEN 64
int sx[MAX_LEN], sy[MAX_LEN], slen;
int sdx, sdy, fx, fy;
bool sOver;

void snakeReset() {
  slen = 5; sdx = 1; sdy = 0; sOver = false;
  for (int i = 0; i < slen; i++) {
    sx[i] = (PLAY_W / CELL) / 2 - i;
    sy[i] = (PLAY_H / CELL) / 2;
  }
  fx = random(PLAY_W / CELL);
  fy = random(PLAY_H / CELL);
}

void snakeUpdate() {
  Button b = readButton();
  if (b == UP && sdy == 0) { sdx = 0; sdy = -1; }
  if (b == DOWN && sdy == 0) { sdx = 0; sdy = 1; }
  if (b == LEFT && sdx == 0) { sdx = -1; sdy = 0; }
  if (b == RIGHT && sdx == 0) { sdx = 1; sdy = 0; }

  for (int i = slen - 1; i > 0; i--) {
    sx[i] = sx[i - 1];
    sy[i] = sy[i - 1];
  }

  sx[0] += sdx;
  sy[0] += sdy;

  if (sx[0] < 0 || sy[0] < 0 ||
      sx[0] >= PLAY_W / CELL || sy[0] >= PLAY_H / CELL)
    sOver = true;

  if (sx[0] == fx && sy[0] == fy) {
    if (slen < MAX_LEN) slen++;
    fx = random(PLAY_W / CELL);
    fy = random(PLAY_H / CELL);
  }

  if (sOver) {
    Animation::flashBorder();
    snakeReset();
    state = MENU;
  }
}

void snakeDraw() {
  u8g2.drawFrame(OFFSET, OFFSET, PLAY_W, PLAY_H);

  for (int i = slen - 1; i >= 1; i--) {
    Animation::drawSnakeSegment(
      OFFSET + sx[i] * CELL,
      OFFSET + sy[i] * CELL,
      false
    );
  }

  Animation::drawSnakeSegment(
    OFFSET + sx[0] * CELL,
    OFFSET + sy[0] * CELL,
    true
  );

  u8g2.drawBox(
    OFFSET + fx * CELL,
    OFFSET + fy * CELL,
    CELL, CELL
  );
}

/* =====================================================
   ================== GAME: PONG =======================
   ===================================================== */
int px, bx, by, bvx, bvy;

void pongReset() {
  px = 50;
  bx = 64; by = 20;
  bvx = 2; bvy = 2;
}

void pongUpdate() {
  Button b = readButton();
  if (b == LEFT)  px -= 5;
  if (b == RIGHT) px += 5;

  bx += bvx;
  by += bvy;

  if (bx <= 4 || bx >= 120) bvx *= -1;
  if (by <= 4) bvy *= -1;

  if (by >= 54) {
    if (bx >= px && bx <= px + 28) bvy *= -1;
    else state = MENU;
  }
}

void pongDraw() {
  u8g2.drawFrame(OFFSET, OFFSET, PLAY_W, PLAY_H);
  u8g2.drawBox(px, 56, 28, 3);
  u8g2.drawBox(bx, by, 3, 3);
}

/* =====================================================
   ================== GAME: DODGE ======================
   ===================================================== */
int dodgePlayerX;
int dodgeBlockX;
int dodgeBlockY;

void dodgeReset() {
  dodgePlayerX = 56;          // center
  dodgeBlockX  = random(8, 112); // random X inside play area
  dodgeBlockY  = 0;
}


void dodgeUpdate() {
  Button b = readButton();

  // Player movement
  if (b == LEFT  && dodgePlayerX > OFFSET + 2)
    dodgePlayerX -= 4;

  if (b == RIGHT && dodgePlayerX < OFFSET + PLAY_W - 14)
    dodgePlayerX += 4;

  // Block falls
  dodgeBlockY += 4;

  // Respawn block if it reaches bottom
  if (dodgeBlockY > OFFSET + PLAY_H) {
    dodgeBlockY = 0;
    dodgeBlockX = random(8, 112);
  }

  // ---- COLLISION CHECK (RECTANGLE OVERLAP) ----
  bool hitX = dodgeBlockX + 6 > dodgePlayerX &&
              dodgeBlockX < dodgePlayerX + 12;

  bool hitY = dodgeBlockY + 6 > 56 &&
              dodgeBlockY < 56 + 4;

  if (hitX && hitY) {
    Animation::flashBorder(2);
    dodgeReset();
    state = MENU;
  }
}

void dodgeDraw() {
  u8g2.drawFrame(OFFSET, OFFSET, PLAY_W, PLAY_H);

  // Player
  u8g2.drawBox(dodgePlayerX, 56, 12, 4);

  // Falling block
  u8g2.drawBox(dodgeBlockX, dodgeBlockY, 6, 6);
}

/* =====================================================
   ================== GAME: MINI DOOM ==================
   ===================================================== */

// Simple 8x8 map (1 = wall)
const uint8_t doomMap[8][8] = {
  {1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,1},
  {1,0,1,0,1,1,0,1},
  {1,0,1,0,0,0,0,1},
  {1,0,0,0,1,0,0,1},
  {1,0,1,0,0,0,0,1},
  {1,0,0,0,0,1,0,1},
  {1,1,1,1,1,1,1,1}
};

void doomReset() {
  doomX = 3.5;
  doomY = 3.5;
  doomA = 0;

  for (int i = 0; i < MAX_ENEMIES; i++) {
    enemies[i].x = random(2, 6);
    enemies[i].y = random(2, 6);
    enemies[i].alive = true;
  }
}

void doomUpdate() {
  Button b = readButton();

  // Movement
  if (b == LEFT)  doomA -= 0.08;
  if (b == RIGHT) doomA += 0.08;

  if (b == UP) {
    doomX += cos(doomA) * 0.1;
    doomY += sin(doomA) * 0.1;
  }
  if (b == DOWN) {
    doomX -= cos(doomA) * 0.1;
    doomY -= sin(doomA) * 0.1;
  }

  // ---- SW5 SHOOT / EXIT ----
  if (b == RESET) {
    if (sw5PressTime == 0) {
      sw5PressTime = millis();
      sw5Held = false;
    }

    if (!sw5Held && millis() - sw5PressTime >= 3000) {
      sw5Held = true;
      sw5PressTime = 0;
      state = MENU;
      return;
    }
  }
  else {
    if (sw5PressTime > 0 && !sw5Held) {
      gunFlash = true;
      gunTime = millis();

      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;

        float ex = enemies[i].x - doomX;
        float ey = enemies[i].y - doomY;
        float angleToEnemy = atan2(ey, ex);
        float angleDiff = fabs(angleToEnemy - doomA);

        if (angleDiff < 0.18) {
          enemies[i].alive = false;
        }
      }
    }
    sw5PressTime = 0;
  }

  // Flash timeout
  if (gunFlash && millis() - gunTime > 80)
    gunFlash = false;
}

void doomDraw() {
  u8g2.drawFrame(OFFSET, OFFSET, PLAY_W, PLAY_H);

  // Raycasting for walls
  for (int x = 0; x < 64; x++) {
    float rayA = doomA - 0.5 + (x / 64.0);
    float dist = 0;

    bool hit = false;
    while (!hit && dist < 8) {
      dist += 0.05;
      int rx = (int)(doomX + cos(rayA) * dist);
      int ry = (int)(doomY + sin(rayA) * dist);
      if (doomMap[ry][rx]) hit = true;
    }

    int h = 40 / (dist + 0.1);
    if (h > 60) h = 60;

    u8g2.drawVLine(
      OFFSET + x * 2,
      32 - h / 2,
      h
    );

    // Draw enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (!enemies[i].alive) continue;

      float dx = enemies[i].x - doomX;
      float dy = enemies[i].y - doomY;
      float dist = sqrt(dx * dx + dy * dy);

      int h = 30 / (dist + 0.1);
      if (h > 40) h = 40;

      int screenX = 64 + (atan2(dy, dx) - doomA) * 40;

      if (screenX > 4 && screenX < 124) {
        u8g2.drawVLine(screenX, 32 - h / 2, h);
      }
    }
  }

  // Gun flash
  if (gunFlash) {
    u8g2.drawBox(56, 46, 16, 10);
  } else {
    u8g2.drawFrame(56, 46, 16, 10);
  }
}

/* =====================================================
   ================== SETUP & LOOP =====================
   ===================================================== */
void setup() {
  Wire.begin(8, 9);
  u8g2.begin();
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  randomSeed(millis());

  bootAnimation();
  state = MENU;

  snakeReset();
  pongReset();
  dodgeReset();
}

void loop() {
  if (readButton() == RESET) {
    state = MENU;
    delay(250);
  }

  u8g2.clearBuffer();

  switch (state) {
    case MENU:
      menuUpdate();
      menuDraw();
      break;

    case SNAKE:
      snakeUpdate();
      snakeDraw();
      break;

    case PONG:
      pongUpdate();
      pongDraw();
      break;

    case DODGE:
      dodgeUpdate();
      dodgeDraw();

      break;

    case DOOM:
      doomUpdate();
      doomDraw();
      break;

    default:
      break;
  }

  u8g2.sendBuffer();
  delay(90);
}