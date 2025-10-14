#ifndef ENEMIES_H
#define ENEMIES_H
  #include <stddef.h>
  #include <stdbool.h>
  #include "gameLogic/bitmaps.h"
  #include "hardware/Nokia5110.h"
  #include "hardware/Timer1.h"
  #include "hardware/Timer2.h"

  typedef struct {
    bool active;
    const Bitmap *sprite, *spriteA, *spriteB;
    int health;
    float xReal, yReal;
    int xPos, yPos;
    float dx, dy;
  } Enemy;

  void Enemies_Init(void);
  Enemy **Get_Enemies(void);
  void Spawn_Enemies(int enemyNumber, int uHealth, const Bitmap *uSpriteA, const Bitmap *uSpriteB, int y, float xVelocity, float yVelocity);
  void Spawn_Enemy(int uHealth, const Bitmap *uSpriteA, const Bitmap *uSpriteB, int x, int y, int xVelocity, int yVelocity);
  void Update_Enemies_Position(void);
#endif