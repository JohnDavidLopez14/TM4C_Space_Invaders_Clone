#include "gameLogic/player.h"

#define DEFAULT_BMP &playerShip0
#define DEFAULT_HEALTH 100

static Player PlayerShip;

void Player_Reset(void){
    PlayerShip.health = DEFAULT_HEALTH;
    PlayerShip.base.xPos = (SCREENW - PlayerShip.base.sprite->width) / 2;     // start at the center of the screen
    PlayerShip.base.yPos = SCREENH - 1;                                  // start at the bottom of the screen   
    PlayerShip.score = 0;
}

void Player_Init(void){
    PlayerShip.base.sprite = DEFAULT_BMP;
    Player_Reset();
}

Player* Get_Player(void){
    return &PlayerShip;
}