#include "gameLogic/explosions.h"
#define UPDATE_PERIOD 0xFFFF

static Explosion Explosions_Storage[MAX_ENEMIES + 1];
static Explosion *Explosions[MAX_ENEMIES + 2]; // 1 for null, 1 for player

Explosion **Get_Explosions(void){
    return Explosions;
}

static void For_All_Active(void(*explosionFunc)(Explosion*)){
    for (Explosion **ptr = Explosions; *ptr != NULL; ptr++){
        if((*ptr)->active)
            explosionFunc(*ptr); // will only call if active
    }
}

static void Set_Update_Flag(void){
    Explosion_Update_Flag = true;
}

void Explosions_Init(void){
    for (int i = 0; i < MAX_ENEMIES + 1; i++){
        Explosions[i] = &Explosions_Storage[i];
    }
    Explosions[MAX_ENEMIES + 1] = NULL;
    Timer3_Init(Set_Update_Flag, UPDATE_PERIOD); // Timer3 default disabled
}

static Explosion *Find_First_Explosion_By_State(bool state){
    for(Explosion **ptr = Explosions; *ptr != NULL; ptr++){
        if((*ptr)->active == state)
            return (*ptr);
    }
    return NULL;
}

static bool Check_If_Any_State(bool state){
    for (Explosion **ptr = Explosions; *ptr != NULL; ptr++){
        if((*ptr)->active == state)
            return true;
    }
    return false;
}

static void Spawn_Explosion_From_Template(Collidable *base, Explosion *template){
    Explosion *current = Find_First_Explosion_By_State(false);
    if (current != NULL){
        if (!Check_If_Any_State(true)){ // if none are active
            Timer3_Enable();
        }
        *current = *template;
        current->active = true;
        current->base.xPos = base->xPos;
        current->base.yPos = base->yPos;
        current->base.sprite = current->frameOne;
        current->currentFrame = frameOne;
    }
}

void Spawn_Enemy_Explosion(Collidable *base){
    Spawn_Explosion_From_Template(base, &Template_EnemyExplosion);
}

void Spawn_Player_Explosion(Collidable *base){
    Spawn_Explosion_From_Template(base, &Template_PlayerExplosion);
}

static void Update_Explosion_Frames_Helper(Explosion *explosion){
    switch(explosion->currentFrame){
        case frameOne: // if on frame one, set to frame two
            explosion->base.sprite = explosion->frameOne;
            explosion->currentFrame = frameTwo;
            break;
        case frameTwo: // if on frame two, set to deactivate
            explosion->active = false;
            explosion->currentFrame = frameNull;
            if (!Check_If_Any_State(true)) // if none are active now
                Timer3_Disable();
            break;
    }
}

void Update_Explosion_Frames(void){
    For_All_Active(Update_Explosion_Frames_Helper);
}

Explosion Template_EnemyExplosion = {
    .active = false,
    .frameOne = &smallExplosion0,
    .frameTwo = &smallExplosion1,
    .currentFrame = frameNull
};

Explosion Template_PlayerExplosion = {
    .active = false,
    .frameOne = &bigExplosion0,
    .frameTwo = &bigExplosion1,
    .currentFrame = frameNull
};