#ifndef ENEMIES_H
#define ENEMIES_H
    #include <stddef.h>
    #include <stdbool.h>
    #include "hardware/Nokia5110.h"
    #include "gameLogic/bitmaps.h"

  typedef struct {
    bool active;
    const Bitmap *sprite;
    int health;
    float xReal, yReal;
    int xPos, yPos;
    float dx, dy;
  } Enemy;

  void Enemies_Init(void);
  Enemy **Get_Enemies(void);
  void Spawn_Enemies(int enemyNumber, int uHealth, const Bitmap *bitmapStruct, int y, float xVelocity, float yVelocity);
  void Spawn_Enemy(int uHealth, const Bitmap *bitmapStruct, int x, int y, int xVelocity, int yVelocity);
#endif