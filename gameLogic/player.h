#ifndef __player_h__
#define __player_h__

// Pre-Processor Includes
#include "gameLogic/bitmaps.h"
#include "gameLogic/collidable.h"
#include "hardware/Nokia5110.h"

// Global Data Types
typedef struct {
    collidable base; // contains the x and y positions on the screen + bitmap struct
    int health;
    int score;
} Player;

//Prototypes
void Player_Init(void);
void Player_Reset(void);
Player* Get_Player(void);

#endif