#ifndef ENEMIES_H
#define ENEMIES_H
  #include <stddef.h>
  #include <stdbool.h>
  #include "gameLogic/bitmaps.h"
  #include "hardware/Nokia5110.h"
  #include "hardware/Timer1.h"
  #include "hardware/Timer2.h"

  typedef struct {
    bool active;
    const Bitmap *sprite, *spriteA, *spriteB;
    int health; // need to add points
    int points;
    int xPos, yPos;
  } Enemy;

  extern const Enemy smallEnemy30Point_Enemy;
  extern const Enemy smallEnemy20Point_Enemy;
  extern const Enemy smallEnemy10Point_Enemy;

  void Enemies_Init(void);
  Enemy **Get_Enemies(void);
  void Spawn_Enemies(int enemyNumber, const Enemy *templateEnemy, int yPos);
  void Spawn_Enemy_From_Template(const Enemy *templateEnemy, int x, int y);
  void Update_Enemies_Position(void);
#endif