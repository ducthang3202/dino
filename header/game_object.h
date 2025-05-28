#pragma once

#include <SDL3/SDL.h>
#include <math.h>

typedef struct Obstacle Obstacle;

typedef enum{
    GROUND,
    AIR
}ObstacleType;

struct Obstacle{
    int sprite;
    int animation_counter;
    SDL_FRect hitbox;
    ObstacleType type;

    // function pointer -> has to be manually assigned to a function in game_object.c upon initialization 
    void (*move)(Obstacle* self);
    void (*draw)(Obstacle* self);
};

typedef struct{
    SDL_FRect hitbox;
    bool mid_air;
    bool ducking;
    bool long_jump;
    int reward;
}Dino;

#include "window.h"

#define DINO_W 88
#define DINO_H 97
#define CACTUS_W 49
#define CACTUS_H 97

extern Dino* dino;
extern SDL_Texture* sprites_txt;
extern float ground;
extern bool dead;
extern int high_score;

void Dino_Move();
void Dino_PassedObstacle(Obstacle* obs);
void Dino_Draw();
void Obstacle_MoveCactus(Obstacle* self);
void Obstacle_MovePterodactyl(Obstacle* self);
void Obstacle_DrawCactus(Obstacle* self);
void Obstacle_DrawPterodactyl(Obstacle* self);
