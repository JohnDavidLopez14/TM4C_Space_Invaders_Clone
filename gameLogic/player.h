#ifndef PLAYER_H
#define PLAYER_H

// Pre-Processor Includes
#include "gameLogic/bitmaps.h"
#include "hardware/Nokia5110.h"

// Global Data Types
typedef struct {
    const Bitmap *sprite;
    unsigned int health;
    unsigned int xPos, yPos;
} Player;

//Prototypes
void Player_Init(void);
Player* Get_Player(void);

#endif