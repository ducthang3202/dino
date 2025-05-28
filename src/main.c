#include "../header/dino_rush.h"


int main(){

    if(CreateGameWindow() != 0){
        return -1;
    }

    bool feedback = DR_Start();
    while(feedback == 0){
        feedback = DR_Start();

        if(feedback == 1){
            // window closed
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    SDL_Log("+ destroyed window/renderer\n");
    SDL_Log("+ quit SDL3\n");
    return 0;
}