#include "gameLogic/enemies.h"

#define MAX_ENEMIES 20
#define ENEMY_MOVEMENT_RELOAD 0x0000FFFF

bool Movement_Flag;

static Enemy EnemyStorage[MAX_ENEMIES];
static Enemy *Enemies[MAX_ENEMIES + 1];

Enemy **Get_Enemies(void){
  return Enemies;
}

Enemy *Find_First_Enemy_By_State(bool state){
  for (int i = 0; Enemies[i] != NULL; i++){
    if (Enemies[i]->active == state)
      return Enemies[i];
  }
  return NULL;
}

// searches for inactive enemy and then spawns it if available
void Spawn_Enemy(int uHealth, const Bitmap *uSpriteA, const Bitmap *uSpriteB, int x, int y, int xVelocity, int yVelocity){
  Enemy *enemy;
  enemy = Find_First_Enemy_By_State(false);
  if (enemy != NULL){
    enemy->active = true;
    enemy->sprite = uSpriteA;
    enemy->spriteA = uSpriteA;
    enemy->spriteB = uSpriteB;
    enemy->health = uHealth;
    enemy->xReal = x;
    enemy->yReal = y;
    enemy->xPos = x;
    enemy->yPos = y;
    enemy->dx = xVelocity;
    enemy->dy = yVelocity;
  }
}

int Enemy_Spacing(int enemyNumber,int enemyWidth, int enemyIndex){
  int enemyTotalWidth = enemyWidth * enemyNumber;
  if (SCREENW < enemyTotalWidth)
    return -1;
  float spacing = (SCREENW - enemyTotalWidth) / (enemyNumber + 1.0f);
  float xPos = spacing * (enemyIndex + 1) + enemyWidth * enemyIndex;
  return (int) (xPos + 0.5f);
}

void Spawn_Enemies(int enemyNumber, int uHealth, const Bitmap *uSpriteA, const Bitmap *uSpriteB, int y, float xVelocity, float yVelocity){
  int enemyWidth = (uSpriteA->width >= uSpriteB->width) ? uSpriteA->width : uSpriteB->width;
  for (int i = 0; i < enemyNumber; i++){
    int xPos = Enemy_Spacing(enemyNumber, enemyWidth, i);
    if (xPos > 0)
      Spawn_Enemy(uHealth, uSpriteA, uSpriteB, xPos, y, xVelocity, yVelocity);
  }
}

// moves all enemies horizontally one pixel
// when a wall is reached, horizontal movement will flip
// enemies will then continue in the other direction
void Update_Enemies_Position(void){

}

void Set_Movement_Flag(void){
  Movement_Flag = true;
}

void Enemies_Init(void){
  for (int i = 0; i < MAX_ENEMIES; i++){
    Enemies[i] = &EnemyStorage[i];
    Enemies[i]->active = false;
    Enemies[i]->sprite = NULL;
    Enemies[i]->spriteA = NULL;
    Enemies[i]->spriteB = NULL;
    Enemies[i]->health = 0;
    Enemies[i]->xReal = 0;
    Enemies[i]->yReal = 0;
    Enemies[i]->xPos = 0;
    Enemies[i]->yPos = 0;
    Enemies[i]->dx = 0;
    Enemies[i]->dy = 0;
  }
  Enemies[MAX_ENEMIES] = NULL;
  Timer2_Init(Set_Movement_Flag, ENEMY_MOVEMENT_RELOAD);
}