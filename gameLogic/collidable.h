#ifndef __collidable_h__
#define __collidable_h__
#include "gameLogic/bitmaps.h"

typedef struct {
    const Bitmap *sprite;
    int xPos;
    int yPos;
} Collidable;

#endif