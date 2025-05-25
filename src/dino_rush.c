#include "../header/dino_rush.h" 

Dino* dino = NULL;
SDL_Texture* sprites_txt = NULL;
SDL_Texture* bg_txt = NULL;
SDL_Surface* bg_s = NULL;
SDL_Surface* sprites_s = NULL;

bool done = false;
bool dead = false;
bool start_animation = false;


int jump_timer = 0;
int retry_animation_counter = 0;
int retry_animation = 0;
int high_score = 0;
bool long_jump = false, jump_down = false;

int DR_Init(){
    // store player on mem heap
    dino = malloc(sizeof(Dino));

    if(!TTF_Init()){
        SDL_Log("- could not init SDL3_TTF lib!\n");
        return -1;
    }
    if(dino == NULL){
        SDL_Log("- could not allocate memory for Player!\n");
        return -1;
    }
    ground = w_bounds.h - w_bounds.h / 10;
    dino->mid_air = false;
    dino->long_jump = false;
    dino->ducking = false;
    dino->reward = 0;
    dino->hitbox = (SDL_FRect){ w_bounds.w / 8, ground - DINO_H, DINO_W, DINO_H};

    bg_s = IMG_Load("assets/desert_pixel.bmp");
    if(bg_s == NULL){
        SDL_Log("- IMG_Load failed: %s", SDL_GetError());
        return -1;
    }

    sprites_s = IMG_Load("assets/base64.bmp");
    if(sprites_s == NULL){
        SDL_Log("- IMG_Load failed: %s", SDL_GetError());
        return -1;
    }

    bg_txt = SDL_CreateTextureFromSurface(renderer, bg_s);
    sprites_txt = SDL_CreateTextureFromSurface(renderer, sprites_s);
    Level_Init();

    return 0;
}

int DR_Start(){

    done = false;
    dead = false;

    if(DR_Init() != 0){
        SDL_Log("- couldnt init Game!\n");
        return -1;
    }

    if(DR_Run() != 0){
        SDL_Log("- Unexpected Game crash!\n");
        return -1;
    } 

    SDL_Log("----------------------------\n");
    SDL_Log("+ Cleanup epilouge\n");
    SDL_Log("----------------------------\n\n");

    if(!LL_Clean(obj_list)){
        SDL_Log("- couldnt properly free Linked List data structure\n");
    }else{
        SDL_Log("+ could properly free Linked List data structure\n");
    }

    free(dino);
    free(obj_list);

    SDL_DestroyTexture(bg_txt);
    SDL_DestroySurface(bg_s);

    SDL_DestroyTexture(sprites_txt);
    SDL_DestroySurface(sprites_s);

    TTF_Quit();

    high_score = 0;

    return 0;
}

int DR_Run(){
  
    int feedback = DR_GameLoop();
    if(feedback == -1){
        SDL_Log("- something went wrong inside GameLoop!\n");
        return -1;
    }

    return feedback;
}


int DR_GameLoop(){

    ULONGLONG prev_time = millis();

    // divide one second into FPS intervals
    float interval = 1000.f / (float)FPS;
    int fps_counter = 0;
    int window_feedback = 0;

    while (!done) {
        
        DR_GameLogic();
        window_feedback = DR_WindowEvents();

        DR_LoopThrottle(&prev_time, interval);
    }

    return window_feedback;
}



void DR_LoopThrottle(ULONGLONG* prev_time, float interval){

    ULONGLONG time = millis();
    ULONGLONG time_passed = time - *prev_time;

    //if loop cycle went too fast for one interval -> sleep for the remainder time
    if(time_passed < interval)
        SDL_Delay(interval - time_passed);
    
    *prev_time = time;
}

void DR_GameLogic(){

    if(!dead){
        Dino_Move();
        dead = Level_Update();
    }

    DR_Draw();
}


void DR_Draw(){

    // background
    SDL_RenderTexture(renderer, bg_txt, NULL, NULL);

    char text_buff[128] = "FPS: ";
    TTF_Font* font = TTF_OpenFont("assets/slkscr.ttf", 40);
    if (!font) {
        SDL_Log("Failed to load font slkscr.ttf: %s", SDL_GetError());
        return;
    }
    int len_var = strlen(text_buff);
    _itoa_s(FPS, &text_buff[len_var], sizeof(text_buff) - len_var, 10);
    SDL_Color black = (SDL_Color) {0, 0, 0};
    SDL_Surface* text_sur = TTF_RenderText_Solid(font, text_buff, 0, black);
    
    if (text_sur) {
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_sur);
        SDL_FRect dst = {10, 10, text_sur->w, text_sur->h}; 
        SDL_RenderTexture(renderer, text_texture, NULL, &dst); 
        SDL_DestroyTexture(text_texture);
    }

    // re-use text buffer
    strcpy(text_buff, "HI: ");
    len_var = strlen(text_buff);
    _itoa_s(high_score, &text_buff[len_var], sizeof(text_buff) - len_var, 10);
    text_sur = TTF_RenderText_Solid(font, text_buff, 0, black);

    if (text_sur) {
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_sur);
        SDL_FRect dst = {w_bounds.w - text_sur->w - 10, 10, text_sur->w, text_sur->h}; 
        SDL_RenderTexture(renderer, text_texture, NULL, &dst); 
        SDL_DestroyTexture(text_texture);
    }

    Dino_Draw();

    if(obj_list->size > 0){
        Node* curr = obj_list->NIL;
        while((curr = curr->next) != obj_list->NIL){
            ((Obstacle*)curr->data)->draw((Obstacle*)curr->data);
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    if(dead){
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 50);
        SDL_FRect screen = (SDL_FRect){0,0,w_bounds.w, w_bounds.h};
        SDL_RenderFillRect(renderer, &screen);

        retry_animation_counter++;
        if(retry_animation_counter > FPS / 3){
            retry_animation = (retry_animation + 1) % 4;
            retry_animation_counter = 0;
        }
            
        SDL_FRect retry_r_src = { 145 + retry_animation * 72, 130 , 72 , 70 };
        SDL_FRect retry_r = { w_bounds.w / 2 - 50 , w_bounds.h / 2 - 50, 72, 70 };
        SDL_RenderTexture(renderer, sprites_txt, &retry_r_src, &retry_r);

       // SDL_RenderRect(renderer,& retry_r);
    }
    // present buffer to screen
    SDL_RenderPresent(renderer);
    SDL_DestroySurface(text_sur);
    
    TTF_CloseFont(font);
}

int DR_WindowEvents(){
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            done = true;
            return 1;
        }
        
        if(event.mdevice.type == SDL_EVENT_MOUSE_BUTTON_DOWN && dead){
            done = true;
            return 0;
        }
    }
    const bool* state = SDL_GetKeyboardState(NULL);
    if(!dino->mid_air)
        dino->ducking = state[SDL_SCANCODE_S];
    else
        dino->ducking = false;
        //SDL_Log("%d\n", dino->ducking);
    if(dino->ducking)
        return 0;

    // either force to long jump after holding jump or low jump by tapping space
    // handling keyboard events manually outside of window event loop for faster response
    // PI/(float)FPS -> smooth animation. increment angle, so that jump animation lasts aprox. one second
    
    if (state[SDL_SCANCODE_SPACE]) {

        jump_down = true; 
        
        jump_timer++;
        if(jump_timer > FPS * 0.12f){
            dino->long_jump = true;
            // printf("Key repeat: \n");
            if(angle == 0)
                angle += PI/(float)FPS;
        }
    }else{
        if(jump_down && angle == 0)
            angle += PI/(float)FPS;

        jump_down = false;
        jump_timer = 0;
    }

    
    return 0;
}

//wrapper
ULONGLONG millis(){
#ifdef WIN
    return GetTickCount64();
#endif
#ifdef MAC
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); 
    return ts.tv_nsec/1000000;
#endif
#ifdef LINUX
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); 
    return ts.tv_nsec/1000000;
#endif

}