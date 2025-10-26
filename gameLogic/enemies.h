#ifndef ENEMIES_H
#define ENEMIES_H
  #include <stddef.h>
  #include <stdbool.h>
  #include "gameLogic/bitmaps.h"
  #include "gameLogic/collidable.h"
  #include "hardware/Nokia5110.h"
  #include "hardware/Timer1.h"
  #include "hardware/Timer2.h"

  typedef struct {
    bool active;
    Collidable base; // contains x and y positions on the screen + bitmap struct
    const Bitmap *spriteA, *spriteB;
    int health; // need to add damage maybe, not sure if this is something I want to add
    int points;
    int dmg;
  } Enemy;

  extern Enemy smallEnemy30Point_Enemy;
  extern Enemy smallEnemy20Point_Enemy;
  extern Enemy smallEnemy10Point_Enemy;

  bool Check_Enemy_End(void);
  void Enemies_Init(void);
  void Enemies_Reset(void);
  Enemy **Get_Enemies(void);
  void Spawn_Enemies(int enemyNumber, const Enemy *templateEnemy, int yPos);
  void Spawn_Enemy_From_Template(const Enemy *templateEnemy, int x, int y);
  void Update_Enemies_Position(void);
#endif