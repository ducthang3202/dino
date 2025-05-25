#pragma once
#include <SDL3/SDL.h>
#include <stdio.h>

#define FPS 120
#define PI M_PI

typedef struct Bounds{
    int w;
    int h;
}Bounds;

extern SDL_Renderer* renderer;
extern SDL_Window* window;

extern Bounds w_bounds;

int CreateGameWindow();