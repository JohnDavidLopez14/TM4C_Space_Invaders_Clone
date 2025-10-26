#include "gameLogic/projectile.h"

#define MAX_MISSILES 10
#define MAX_LASERS 10
#define MISSILEV .05f // pixel/tick
#define LASERV .05f // pixel/tick
#define DEFAULT_LASER_BMP &laser0
#define DEFAULT_MISSILE_BMP &missile0

static Projectile MissileStorage[MAX_MISSILES];
static Projectile *Missiles[MAX_MISSILES + 1];
static Projectile *Lasers[MAX_LASERS + 1];
static Projectile LaserStorage[MAX_LASERS];
static Player *PlayerShip;

static void For_All_Missiles(void missileFunc(Projectile*)){
  for (Projectile **ptr = Missiles; *ptr != NULL; ptr++){
    missileFunc(*ptr);
  }
}

static void For_All_Lasers(void laserFunc(Projectile*)){
  for(Projectile **ptr = Lasers; *ptr != NULL; ptr++){
    laserFunc(*ptr);
  }
}

static void Projectile_Reset_Helper(Projectile *projectile){
  projectile->active = false;
}

void Projectile_Reset(void){
  For_All_Missiles(Projectile_Reset_Helper);
  For_All_Lasers(Projectile_Reset_Helper);
}

void Projectile_Init(Player *playerShip){
  // store player bitmap into global
  PlayerShip = playerShip;

  // Initialize missile array
  for (int i = 0; i < MAX_MISSILES; i++){
    Missiles[i] = &MissileStorage[i];
    Missiles[i]->base.sprite = &missile0;
    Missiles[i]->active = false;
  }
  Missiles[MAX_MISSILES] = NULL;

  // Initialize laser array
  for (int i = 0; i < MAX_LASERS; i++){
    Lasers[i] = &LaserStorage[i];
    Lasers[i]->base.sprite = &laser0;
    Lasers[i]->active = false;
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
static Projectile *Find_First_Projectile_By_State(Projectile **projectileList, bool state){
  for (int i = 0;projectileList[i] != NULL; i++){
    if (projectileList[i]->active == state){
      return projectileList[i];
    } 
  }
  return NULL;
}

static void Fire_Projectile(Projectile **projectileList, float velocity, const Bitmap *bitmapStruct){
  Projectile *projectile;
  projectile = Find_First_Projectile_By_State(projectileList, false); // search for inactive projectile
  if (projectile != NULL){
    projectile->base.sprite = bitmapStruct;
    projectile->active = true;
    projectile->base.xPos = PlayerShip->base.xPos + (PlayerShip->base.sprite->width - projectile->base.sprite->width) / 2; //center the projectile on the ship
    projectile->base.yPos = PlayerShip->base.yPos - PlayerShip->base.sprite->height;
		projectile->yReal = projectile->base.yPos;
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

static void Update_Projectile_Position(Projectile *projectile){
  if(projectile->active == true){
    projectile->yReal -= projectile->dy; // subtract dy every frame, each tick is t, ie y0 - dy * dt
    projectile->base.yPos = (int)(projectile->yReal + 0.5f); // round up and then type cast
  }
}

void Update_Missile_Position(void){
  For_All_Missiles(Update_Projectile_Position);
}

void Update_Laser_Position(void){
  For_All_Lasers(Update_Projectile_Position);
}

static bool Is_Out_Of_Bounds(Projectile *projectile, int width, int height){
  return (
    projectile->base.xPos < 0 || // if the bitmap is to the left of the screen
    projectile->base.xPos + projectile->base.sprite->width > width || // if the bitmap is to the right of the screen
    projectile->base.yPos > height || // if the bitmap is below the bottom edge
    projectile->base.yPos + projectile->base.sprite->height < 0 // if the sprite is above the screen
  );
}

static void Check_Projectile_OOB(Projectile *projectile){
  if (projectile -> active){
    if (Is_Out_Of_Bounds(projectile, SCREENW, SCREENH)){
      projectile->active = false;
    }
  }
}

void Check_Missiles_OOB(void){
  For_All_Missiles(Check_Projectile_OOB);
}

void Check_Lasers_OOB(void){
  For_All_Lasers(Check_Projectile_OOB);
}