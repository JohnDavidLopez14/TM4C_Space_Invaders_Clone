#include "gameLogic/enemies.h"
#include "hardware/UART.h"
#include "hardware/Timer0.h"

#define MAX_ENEMIES 20
#define ENEMY_MOVEMENT_RELOAD 0x1E84800

static volatile bool Movement_Flag = false;
static volatile bool VERTICAL_MOVEMENT_FLAG = true;
static volatile bool SpriteA_Flag = true;
static volatile int Movement_Dir = 1;

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
void Spawn_Enemy_From_Template(const Enemy *templateEnemy, int x, int y){
  Enemy *enemy;
  enemy = Find_First_Enemy_By_State(false);
  if (enemy != NULL){
    *enemy = *templateEnemy; // copy all fields from the template
    enemy->active = true;
    enemy->xPos = x;
    enemy->yPos = y;
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

void Spawn_Enemies(int enemyNumber, const Enemy *templateEnemy, int yPos){
  int enemyWidth = (templateEnemy->spriteA->width >= templateEnemy->spriteB->width) ? templateEnemy->spriteA->width : templateEnemy->spriteB->width;
  for (int i = 0; i < enemyNumber; i++){
    int xPos = Enemy_Spacing(enemyNumber, enemyWidth, i);
    if (xPos > 0)
      Spawn_Enemy_From_Template(templateEnemy, xPos, yPos);
  }
}

void Move_Horizontally_One_Pixel(){
    for (int i = 0; Enemies[i] != NULL; i++){
      if (Enemies[i]->active)
        Enemies[i]->xPos += Movement_Dir;
  }
}

void Move_Down_One_Pixel(){
  Movement_Dir *= -1; // switch horizontal movement
  for (int i = 0; Enemies[i] != NULL; i++){
    if(Enemies[i]->active)
      Enemies[i]->yPos += 1;
  }

}

bool Check_Vertical_Movement(void){
  for (int i = 0; Enemies[i] != NULL; i++){
    if (Enemies[i]->active)
      if (Enemies[i]->xPos == 0 || (Enemies[i]->xPos + Enemies[i]->sprite->width) == SCREENW) // if the enemy is off the screen on either side
        return true;
  }
  return false;
}

void Swap_SpriteAB(void){
  if (SpriteA_Flag) {
    for (int i = 0; Enemies[i] != NULL; i++){
      Enemies[i]->sprite = Enemies[i]->spriteB;
    }
    SpriteA_Flag = false;
  } else {
    for (int i = 0; Enemies[i] != NULL; i++){
      Enemies[i]->sprite = Enemies[i]->spriteA;
    }
    SpriteA_Flag = true;
  }
}

// moves all enemies horizontally one pixel
// when a wall is reached, horizontal movement will flip
// enemies will then continue in the other direction
void Update_Enemies_Position(void){
  if (!Movement_Flag)
    return;
  Movement_Flag = false; // reset flag
  Swap_SpriteAB();
  if (Check_Vertical_Movement() && VERTICAL_MOVEMENT_FLAG){
    Move_Down_One_Pixel();
    VERTICAL_MOVEMENT_FLAG = false;
  } else {
    Move_Horizontally_One_Pixel();
    VERTICAL_MOVEMENT_FLAG = true;
  }
}

void Set_Movement_Flag(void){
  Movement_Flag = true;
}

void Enemies_Reset(void){
  for (Enemy **ptr = Enemies;*ptr != NULL; ptr++){
    (*ptr)->active = false;
  }
}

void Enemies_Init(void){
  for (int i = 0; i < MAX_ENEMIES; i++){
    Enemies[i] = &EnemyStorage[i];
  }
  Enemies[MAX_ENEMIES] = NULL;
  Timer1_Init(Set_Movement_Flag, ENEMY_MOVEMENT_RELOAD);
}

const Enemy smallEnemy30Point_Enemy = {false, &smallEnemy30PointA, &smallEnemy30PointA, &smallEnemy30PointB, 100, 30, 0, 0};
const Enemy smallEnemy20Point_Enemy = {false, &smallEnemy20PointA, &smallEnemy20PointA, &smallEnemy20PointB, 100, 20 ,0 ,0};
const Enemy smallEnemy10Point_Enemy = {false, &smallEnemy10PointA, &smallEnemy10PointA, &smallEnemy10PointB, 100, 10, 0, 0};