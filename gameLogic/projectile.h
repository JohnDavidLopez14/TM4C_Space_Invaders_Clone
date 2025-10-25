#ifndef PROJECTILE_H
#define PROJECTILE_H

// Pre-Processor Includes
#include <stddef.h>
#include <stdbool.h>
#include "hardware/Nokia5110.h"
#include "gameLogic/bitmaps.h"
#include "gameLogic/player.h"

// Global Data Types
typedef struct {
  const Bitmap *sprite;
  bool active;
  unsigned int xPos, yPos; // stores the screen position
	float xReal, yReal; // stores the actual position
  float dy; // this will be pixels/tick
} Projectile;

// Prototypes
void Projectile_Init(Player *);
void Projectile_Reset(void);
Projectile **Get_Missiles(void);
Projectile **Get_Lasers(void);
void Fire_Missile(void);
void Fire_Laser(void);
void Update_Missile_Position(void);
void Update_Laser_Position(void);
void Check_Missiles_OOB(void);
void Check_Lasers_OOB(void);

#endif