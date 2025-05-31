#pragma once

#include <SDL3/SDL.h>
#include <math.h>


typedef struct Obstacle Obstacle;
typedef struct GameObject GameObject;

typedef enum{
    GROUND,
    AIR
}ObstacleType;

// simulate polymorphism ins c
struct GameObject{
    // function pointer -> has to be manually assigned to a function in game_object.c upon initialization 
    // in java/C# the object instance is passed implicitly
    // essential when iterating over objects generically in level.c
    void (*move)(GameObject* self);
    void (*draw)(GameObject* self);

    // cant pass Dino* or Obstacle* as GamoObject* directly
    // has to be manually casted to childclass type
    void* childclass_ref;

    SDL_FRect hitbox;

    // passed pointer to all sprites
    SDL_Texture* sprites_txt;
};

struct Obstacle{

    GameObject go;

    int sprite;
    int animation_counter;
    
    ObstacleType type;
};

typedef struct{
    GameObject go;
    bool mid_air;
    bool ducking;
    bool long_jump;
    bool dead;
    int reward;
    float jump_angle;
}Dino;

#include "window.h"

#define DINO_W 88
#define DINO_H 97
#define CACTUS_W 49
#define CACTUS_H 97

extern int high_score;
extern bool show_hitbox;
extern float actual_fps;

void Dino_Move(GameObject* go);
void Dino_PassedObstacle(Dino* dino,Obstacle* obs);
void Dino_Draw(GameObject* go);
void Obstacle_MoveCactus(GameObject* go);
void Obstacle_MovePterodactyl(GameObject* go);
void Obstacle_DrawCactus(GameObject* go);
void Obstacle_DrawPterodactyl(GameObject* go);
