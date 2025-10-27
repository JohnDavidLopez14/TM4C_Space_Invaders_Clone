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

// iterate through all enemies, does not return anything
static void For_All(void (*enemyFunc)(Enemy*)){
  for (Enemy **ptr = Enemies; *ptr != NULL; ptr++){
    enemyFunc(*ptr);
  }
}

static Enemy *Find_First_Enemy_By_State(bool state){
  for (Enemy **ptr = Enemies; *ptr != NULL; ptr++){
    if ((*ptr)->active == state)
      return (*ptr);
  }
  return NULL;
}

bool Check_Enemy_End(void){
  for (Enemy **ptr = Enemies; *ptr != NULL; ptr++){
    Enemy *enemy = *ptr;
    if (enemy->active){
      if (enemy->base.yPos > SCREENH)
        return true;
    }
  }
  return false;
}

// searches for inactive enemy and then spawns it if available
void Spawn_Enemy_From_Template(const Enemy *templateEnemy, int x, int y){
  Enemy *enemy;
  enemy = Find_First_Enemy_By_State(false);
  if (enemy != NULL){
    *enemy = *templateEnemy; // copy all fields from the template
    
    // set active and set initial position
    enemy->active = true;
    enemy->base.xPos = x;
    enemy->base.yPos = y;
  }
}

static int Enemy_Spacing(int enemyNumber,int enemyWidth, int enemyIndex){
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

static void Move_Horizontally_One_Pixel_Helper(Enemy *enemy){
  enemy->base.xPos += Movement_Dir;
}

static void Move_Horizontally_One_Pixel(void){
  For_All(Move_Horizontally_One_Pixel_Helper);
}

static void Move_Down_One_Pixel_Helper(Enemy *enemy){
  if (enemy->active)
    enemy->base.yPos += 1;
}

static void Move_Down_One_Pixel(void){
  Movement_Dir *= -1; // switch horizontal movement
  For_All(Move_Down_One_Pixel_Helper);
}

static bool Check_Vertical_Movement(void){
  for (Enemy **ptr = Enemies; *ptr != NULL; ptr++){
    if ((*ptr)->active)
      if ((*ptr)->base.xPos == 0 || (*ptr)->base.xPos + (*ptr)->base.sprite->width == SCREENW) // if the enemy is off the screen on either side
        return true;
  }
  return false;
}

static void Swap_SpriteA(Enemy *enemy){
  enemy->base.sprite = enemy->spriteA;
}

static void Swap_SpriteB(Enemy *enemy){
  enemy->base.sprite = enemy->spriteB;
}

static void Swap_SpriteAB(void){
  if (SpriteA_Flag) {
    For_All(Swap_SpriteB);
    SpriteA_Flag = false;
  } else {
    For_All(Swap_SpriteA);
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

static void Set_Movement_Flag(void){
  Movement_Flag = true;
}

static void Enemies_Reset_Helper(Enemy *enemy){
  enemy->active = false;
}

void Enemies_Reset(void){
  For_All(Enemies_Reset_Helper);
}

void Enemies_Init(void){
  for (int i = 0; i < MAX_ENEMIES; i++){
    Enemies[i] = &EnemyStorage[i];
  }
  Enemies[MAX_ENEMIES] = NULL;
  Timer1_Init(Set_Movement_Flag, ENEMY_MOVEMENT_RELOAD);
}

Enemy smallEnemy30Point_Enemy = {
  .active = false,
  .base = {.sprite = &smallEnemy30PointA, .xPos = 0, .yPos = 0},
  .spriteA = &smallEnemy30PointA,
  .spriteB = &smallEnemy30PointB,
  .health = 100,
  .points = 30,
  .dmg = 100
};

Enemy smallEnemy20Point_Enemy = {
  .active = false,
  .base = {.sprite = &smallEnemy20PointA, .xPos = 0, .yPos = 0},
  .spriteA = &smallEnemy20PointA,
  .spriteB = &smallEnemy20PointB,
  .health = 100,
  .points = 20,
  .dmg = 100
};

Enemy smallEnemy10Point_Enemy = {
  .active = false,
  .base = {.sprite = &smallEnemy20PointA, .xPos = 0, .yPos = 0},
  .spriteA = &smallEnemy20PointA,
  .spriteB = &smallEnemy20PointB,
  .health = 100,
  .points = 20,
  .dmg = 100
};