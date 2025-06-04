#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

int main() {
    // Initialize SDL video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    
    // Create a window - SDL3 uses different parameters
    SDL_Window* window = SDL_CreateWindow("SDL3 Test", 800, 600, SDL_WINDOW_OPENGL);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Create a renderer - SDL3 simplified this
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Set a background color (optional)
    SDL_SetRenderDrawColor(renderer, 64, 128, 255, 255); // Light blue
    
    // Clear the renderer and present it
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    // Simple event loop instead of just delay
    SDL_Event e;
    bool quit = false;
    Uint64 startTime = SDL_GetTicks();
    
    while (!quit && (SDL_GetTicks() - startTime) < 2000) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
        SDL_Delay(16); // ~60 FPS
    }
    
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}