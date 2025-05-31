#include "../header/dino_rush.h"

int main(){

    if(DESIRED_FPS > 700){
        printf("- cant run the game if fps is greater than 700\n");
        Sleep(5000);
        return -1;
    }
    SDL_Window* window = NULL;

    if( CreateGameWindow(window) != 0){
        return -1;
    }

    while(DR_Start() == 0);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    SDL_Log("+ destroyed window/renderer\n");
    SDL_Log("+ quit SDL3\n");
    return 0;
}