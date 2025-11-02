#ifndef EXPLOSION_H
#define EXPLOSION_H

// Pre-Processor Inclueds
#include <stdbool.h>
#include "gameLogic/collidable.h"
#include "gameLogic/enemies.h"
#include "hardware/Timer3.h"

typedef enum {frameOne_enum, frameTwo_enum} Explosion_Frame;

// Global Data Types
typedef struct {
    bool active;
    Collidable base;
    const Bitmap *frameOne, *frameTwo;
    Explosion_Frame currentFrame;
} Explosion;

// Flags
extern volatile bool Explosion_Update_Flag;

// Prototypes
void Explosions_Init(void);
Explosion **Get_Explosions(void);
void Spawn_Enemy_Explosion(Collidable *);
void Spawn_Player_Explosion(Collidable *);
void Update_Explosion_Frames(void);

#endif