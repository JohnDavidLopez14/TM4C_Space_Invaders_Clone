#include "gameLogic/enemies.h"

#define MAX_ENEMIES 20

static Enemy EnemyStorage[MAX_ENEMIES];
static Enemy *Enemies[MAX_ENEMIES + 1];

void Enemies_Init(void){
  for (int i = 0; i < MAX_ENEMIES; i++){
    Enemies[i] = &EnemyStorage[i];
    Enemies[i]->active = false;
    Enemies[i]->sprite = NULL;
    Enemies[i]->health = 0;
    Enemies[i]->xReal = 0;
    Enemies[i]->yReal = 0;
    Enemies[i]->xPos = 0;
    Enemies[i]->yPos = 0;
    Enemies[i]->dx = 0;
    Enemies[i]->dy = 0;
  }
  Enemies[MAX_ENEMIES] = NULL;
}

// searches for inactive enemy and then spawns it if available
void Spawn_Enemy(int uHealth, Bitmap *bitmapStruct, int x, int y, int xVelocity, int yVelocity){
  for (int i = 0; Enemies[i] != NULL; i++){
    if (Enemies[i] && Enemies[i]->active == true){
      Enemies[i]->health = uHealth;
      Enemies[i]->sprite = bitmapStruct;
      Enemies[i]->xReal = x;
      Enemies[i]->xPos = x;
      Enemies[i]->yReal = y;
      Enemies[i]->yPos = y;
      Enemies[i]->dx = xVelocity;
      Enemies[i]->dy = yVelocity;
      return;
    }
  }
}

void Enemy_Spacing(int enemyNumber,int enemyTotalWidth , int enemyIndex){

}

void Spawn_Enemies(int enemyNumber, int uHealth, Bitmap *bitmapStruct, int y, int xVelocity, int yVelocity){
  for (int i = 0; i < enemyNumber; i++){
    Spawn_Enemy(uHealth, bitmapStruct, 911, y, xVelocity, yVelocity);
  }
}