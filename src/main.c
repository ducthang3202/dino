#include "../header/dino_rush.h"

int main(){
    if(DESIRED_FPS > 700){
        printf("- cant run the game if fps is greater than 700\n");
        #ifdef _WIN32
        Sleep(5000);  // Windows: Sleep takes milliseconds
        #else
        usleep(5000 * 1000);  // macOS/Linux: usleep takes microseconds
        #endif
        return -1;
    }
    
    SDL_Window* window = NULL;
    
    if(CreateGameWindow(&window) != 0){
        return -1;
    }
    
    while(DR_Start() == 0);
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    // Add TTF cleanup here
    TTF_Quit();
    SDL_Log("+ disposed SDL3_TTF\n");
    
    SDL_Quit();
    
    SDL_Log("+ destroyed window/renderer\n");
    SDL_Log("+ quit SDL3\n");
    return 0;
}