#ifndef ENEMIES_H
#define ENEMIES_H
    #include <stddef.h>
    #include <stdbool.h>
    #include "gameLogic/bitmaps.h"

  typedef struct {
    bool active;
    const Bitmap *sprite;
    int health;
    float xReal, yReal;
    int xPos, yPos;
    int dx, dy;
  } Enemy;
#endif