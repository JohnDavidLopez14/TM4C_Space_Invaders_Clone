#include "gameLogic/projectile.h"

#define MAX_MISSILES 4
#define MAX_LASERS 4
#define MISSILEV .05 // pixsel/tick
#define LASERV .05 // pixel/tick
#define DEFAULT_LASER_BMP &laser0
#define DEFAULT_MISSILE_BMP &missile0

static Projectile MissileStorage[MAX_MISSILES];
static Projectile *Missiles[MAX_MISSILES + 1];
static Projectile *Lasers[MAX_LASERS + 1];
static Projectile LaserStorage[MAX_LASERS];
static Player *PlayerShip;

void Projectile_Init(Player *playerShip){
  // store player bitmap into global
  PlayerShip = playerShip;

  // Initialize missile array
  for (int i = 0; i < MAX_MISSILES; i++){
    Missiles[i] = &MissileStorage[i];
    Missiles[i]->sprite = &missile0;
    Missiles[i]->active = false;
    Missiles[i]->xPos = 0;
    Missiles[i]->yPos = 0;
    Missiles[i]->xReal = 0;
    Missiles[i]->yReal = 0;
    Missiles[i]->dy = 0;
  }
  Missiles[MAX_MISSILES] = NULL;

  // Initialize laser array
  for (int i = 0; i < MAX_LASERS; i++){
    Lasers[i] = &LaserStorage[i];
    Lasers[i]->sprite = &laser0;
    Lasers[i]->active = false;
    Lasers[i]->xPos = 0;
    Lasers[i]->yPos = 0;
    Lasers[i]->xReal = 0;
    Lasers[i]->yReal = 0;
    Lasers[i]->dy = 0;
  }
  Lasers[MAX_LASERS] = NULL;
}

// return pointer to a null terminated array
Projectile **Get_Missiles(void){
  return Missiles;
}

// return pointer to a null terminated array
Projectile **Get_Lasers(void){
  return Lasers;
}

// searches projectile list for active status
// bool - 1 for active, 0 for inactive
// returns a NULL if nothing is found, a projectile pointer if found
Projectile *Projectile_Search(Projectile **projectileList, bool state){
  for (int i = 0;projectileList[i] != NULL; i++){
    if (projectileList[i]->active == state){
      return projectileList[i];
    } 
  }
  return NULL;
}

void Fire_Projectile(Projectile **projectileList, float velocity, const Bitmap *bitmapStruct){
  Projectile *projectile;
  projectile = Projectile_Search(projectileList, false); // search for inactive projectile
  if (projectile != NULL){
    projectile->sprite = bitmapStruct;
    projectile->active = true;
    projectile->xPos = PlayerShip->xPos + (PlayerShip->sprite->width - projectile->sprite->width) / 2; //center the projectile on the ship
    projectile->yPos = PlayerShip->yPos - PlayerShip->sprite->height;
		projectile->xReal = projectile->xPos;
		projectile->yReal = projectile->yPos;
    projectile->dy = velocity;
  }
}

// Fires missile
void Fire_Missile(void){
  Fire_Projectile(Missiles, MISSILEV, DEFAULT_MISSILE_BMP);
}

// Fires laser
void Fire_Laser(void){
  Fire_Projectile(Lasers, LASERV, DEFAULT_LASER_BMP);
}

void Update_Projectile_Position(Projectile **projectileList){
  for (int i = 0;projectileList[i] != NULL; i++){
    if(projectileList[i]->active == true){ // iterate through active projectiles
      projectileList[i]->yReal -= projectileList[i]->dy; // subtract dy every frame, each tick is t, ie y0 - dy * dt
      projectileList[i]->yPos = (int)(projectileList[i]->yReal + 0.5f); // round up and then type cast
    }
  }
}

void Update_Missile_Position(void){
  Update_Projectile_Position(Missiles);
}

void Update_Laser_Position(void){
  Update_Projectile_Position(Lasers);
}

unsigned int Is_Out_Of_Bounds(unsigned int x, unsigned int y, unsigned int width, unsigned int height){
  return (x > width || y > height );
}

void Check_Projectile_OOB(Projectile **projectileList){
  for (int i = 0;projectileList[i] != NULL; i++){
    if (projectileList[i]->active){
      if (Is_Out_Of_Bounds(projectileList[i]->xPos, projectileList[i]->yPos, SCREENW, SCREENH)){
        projectileList[i]->active = false;
      }
    }
  }
}

void Check_Missiles_OOB(void){
  Check_Projectile_OOB(Missiles);
}

void Check_Lasers_OOB(void){
  Check_Projectile_OOB(Lasers);
}