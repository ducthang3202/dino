#pragma once
#include <SDL3/SDL.h>
#include <stdio.h>
#include "dino_rush.h"

#define FPS 120
#define PI M_PI
#define GAME_WINDOW_EVENT_NOERR 0
#define GAME_WINDOW_EVENT_EXIT_SAFE 1
#define GAME_WINDOW_EVENT_EXIT_CRASH -1

typedef struct Bounds{
    int w;
    int h;
}Bounds;

extern SDL_Renderer* renderer;
extern SDL_Window* window;
extern bool done;
extern Bounds w_bounds;

int CreateGameWindow();
int WindowEvents();