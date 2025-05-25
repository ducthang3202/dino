#include "../header/window.h"

SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;
Bounds w_bounds = {1000, 600};


int CreateGameWindow(){

    if(!SDL_Init(SDL_INIT_VIDEO)){

        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "- Could not initialize SDL3: %s\n", SDL_GetError());
        return -1;
    }            

    window = SDL_CreateWindow(
        "DinoRush",                         // window title
        1000,                               // width, in pixels
        600,                                // height, in pixels
        SDL_WINDOW_VULKAN                   // renderer
    );

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {

        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}