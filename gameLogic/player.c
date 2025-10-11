#include "gameLogic/player.h"

#define DEFAULT_BMP &playerShip0
#define DEFAULT_HEALTH 100

Player PlayerShip;

void Player_Init(void){
    PlayerShip.sprite = DEFAULT_BMP;
    PlayerShip.health = DEFAULT_HEALTH;
    PlayerShip.xPos = (SCREENW - PlayerShip.sprite->width) / 2;     // start at the center of the screen
    PlayerShip.yPos = SCREENH - 1;                                  // start at the bottom of the screen   
}

Player* Get_Player(void){
    return &PlayerShip;
}