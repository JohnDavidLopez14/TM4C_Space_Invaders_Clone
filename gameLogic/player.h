#ifndef PLAYER_H
#define PLAYER_H

// Pre-Processor Includes
#include "gameLogic/bitmaps.h"
#include "hardware/Nokia5110.h"

// Global Data Types
typedef struct {
    const Bitmap *sprite;
    int health;
    unsigned int xPos, yPos;
    int score;
} Player;

//Prototypes
void Player_Init(void);
void Player_Reset(void);
Player* Get_Player(void);

#endif