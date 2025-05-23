#include "../header/window.h"

int CreateGameWindow(SDL_Window* window){

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not initialize SDL3: %s\n", SDL_GetError());
        return 1;
    }            

    window = SDL_CreateWindow(
        "DinoDash",                         // window title
        1000,                               // width, in pixels
        600,                                // height, in pixels
        SDL_WINDOW_OPENGL                   // renderer
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_FRect rect = { 0, 0, 20, 20 };

    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_RenderClear(renderer);
        //outline rect
        SDL_SetRenderDrawColor(renderer, 0 , 0, 0, 255);
        //fill up rectangle with color
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);

    if (window == NULL) {

        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }
}