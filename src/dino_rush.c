#include "../header/dino_rush.h" 

Dino* dino = NULL;
SDL_Texture* sprites_txt = NULL;
SDL_Texture* bg_txt = NULL;
TTF_Font* font = NULL;

bool done = false;
bool dead = false;

int high_score = 0, total_high_score = 0;
bool show_hitbox = false;
bool long_jump = false;
bool run_title_screen = true;
float actual_fps = DESIRED_FPS;

int DR_Run();
int DR_GameLoop();
int DR_Init();
Bounds DR_DrawText(int x, int y, TextLayout layout, SDL_Color col, char* buff );
void DR_LoopThrottle();
void DR_GameLogic();
void DR_Draw();
void DR_RenderText(int x, int y, SDL_Color col, TextLayout layout, SDL_Surface* text_sur);
void DR_ShowFPS();

int DR_TitleScreen(){
 
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    float feedback = 0, flashing_animation_counter = 0, y_pos = 0, end_controls_x = 0, end_logo_y = 0, end_press_any_key_y = 0, delay = 0;
    float alpha_logo = 0, alpha_press_start = 0;
    bool animation_finish = false, start_end_sequence = false;
    GameWindowEvent gwe = (GameWindowEvent) { false, SDL_SCANCODE_SPACE};

    while(run_title_screen){
        
        // regulation of title animation
        // 1.fading 
        // 2.moving to the top
        // 3.flashing
        if(alpha_logo < 255.f && flashing_animation_counter == 0)
            alpha_logo += 160.f/actual_fps;
        else if(w_bounds.h/2 + y_pos > w_bounds.h / 8 && flashing_animation_counter == 0){
            alpha_logo = 255.f;
            y_pos -= 160.f/actual_fps;
        }
        else if(flashing_animation_counter < actual_fps){
            flashing_animation_counter++;
        }else{
            alpha_press_start = 255.f - alpha_press_start;
            flashing_animation_counter = 1;
            animation_finish = true;
        }
        
        SDL_RenderTexture(renderer, bg_txt, NULL, NULL);

        TTF_SetFontSize(font, 80);
        DR_DrawText(w_bounds.w/2, w_bounds.h/2 + y_pos + end_logo_y, RIGHT | CENTER_Y, (SDL_Color) {0xff, 0x33, 0x00, (Uint8)alpha_logo}, "Dino");
        DR_DrawText(w_bounds.w/2, w_bounds.h/2 + y_pos+ end_logo_y, LEFT | CENTER_Y, (SDL_Color) {255, 0, 0, (Uint8)alpha_logo}, "Rush");

        if(animation_finish){
            
            //TTF_SetFontSize(font, 60);

            Bounds b = DR_DrawText(end_controls_x, w_bounds.h/2 - w_bounds.h/6, LEFT | CENTER_Y, (SDL_Color) {128, 128, 128, 255}, "Controls:");
          //  TTF_SetFontSize(font, 40);
            Bounds txt_b = DR_DrawText(end_controls_x, w_bounds.h/2 - w_bounds.h/6 + txt_b.h, LEFT | CENTER_Y, (SDL_Color) {0, 0, 0, 255}, "- Hold [SPACE] to jump high");
            b.w = txt_b.w;
            txt_b.h += DR_DrawText(end_controls_x, w_bounds.h/2 - w_bounds.h/6 + txt_b.h, LEFT | CENTER_Y, (SDL_Color) {0, 0, 0, 255}, "- Press [SPACE] to jump low").h;
            txt_b.h += DR_DrawText(end_controls_x, w_bounds.h/2 - w_bounds.h/6 + txt_b.h, LEFT | CENTER_Y, (SDL_Color) {0, 0, 0, 255}, "- Press [S] to duck").h;
            txt_b.h += DR_DrawText(end_controls_x, w_bounds.h/2 - w_bounds.h/6 + txt_b.h, LEFT | CENTER_Y, (SDL_Color) {0, 0, 0, 255}, "- Press [H] to show every Hitbox").h;

          //  TTF_SetFontSize(font, 50);
            Bounds txt_pak = DR_DrawText(w_bounds.w/2 - 20, w_bounds.h - w_bounds.h/5 + end_press_any_key_y, CENTER, (SDL_Color) {0x2E, 0x60, 0x21, (Uint8)alpha_press_start}, "Press SPACE Key");

            // get input after animation is finished
            // once SPACE is pressed hold animation via || start_end_sequence
            // until loop is closed
            if(gwe.key_pressed || start_end_sequence){
                start_end_sequence = true;

                // braucht am längsten
                if(end_controls_x > -txt_b.w - 50){
                    end_controls_x -= 280.f/actual_fps;
                }else
                    run_title_screen = false;
                if(w_bounds.h - w_bounds.h/5 + end_press_any_key_y < w_bounds.h + txt_pak.h )
                    end_press_any_key_y += 160.f/actual_fps;
                if(w_bounds.h/2 - w_bounds.h/6 + end_logo_y > -b.h)
                    end_logo_y -= 160.f/actual_fps;
            }
            delay++;
        }

        feedback = WindowEvents(&gwe);

        DR_ShowFPS();
        
        if(!animation_finish || delay > 10)
            SDL_RenderPresent(renderer);

        DR_LoopThrottle();
    }

    TTF_SetFontSize(font, 40);

    return feedback;
}

void DR_ShowFPS(){
    
    // static -> declared once in static memory (.bss section)
    // every call reuses same memory
    // value only assigned once
    static Uint64 prev_time_true_fps = 0;
    static int true_fps_counter = 0;
    static int true_fps = 0;
    static bool optimizing_fps = false;
    static int wait_changes = 0;

    Uint64 ticks_now = SDL_GetPerformanceCounter();
    true_fps_counter++;
  //  SDL_Log("%llu \n", millis() - prev_time_true_fps);
    if(ticks_now - prev_time_true_fps > SDL_GetPerformanceFrequency()){
        prev_time_true_fps = ticks_now; 

#ifdef ALLOW_FPS_OPTIMIZING
        if(wait_changes < 2){
            wait_changes++;
        }  
        else{
            optimizing_fps = false;
            if(true_fps_counter < DESIRED_FPS - 10 && true_fps_counter > DESIRED_FPS - DESIRED_FPS*0.2f){
                actual_fps += DESIRED_FPS - 10 - true_fps_counter;
                optimizing_fps = true;
            }

            if(true_fps_counter > DESIRED_FPS + 10 && true_fps_counter < DESIRED_FPS + DESIRED_FPS*0.2f){
                optimizing_fps = true;
                actual_fps -= 5;
            }
            wait_changes = 0;
        }
#endif       
        true_fps = true_fps_counter;
        true_fps_counter = 0; 
    }

    TTF_SetFontSize(font, 40);

    char buff[128];
    snprintf(buff, sizeof(buff), "FPS: %d", true_fps);
    Bounds b = DR_DrawText(0, 0, LEFT, (SDL_Color) {0, 0, 0, 255} ,buff);   
  //  if(optimizing_fps){
 //       snprintf(buff, sizeof(buff), "OPTIMZING" );
  //      DR_DrawText(0, b.h, LEFT, (SDL_Color) {0, 255, 0, 200} ,buff);   
  //  }

}

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
   
    font = TTF_OpenFont("assets/slkscr.ttf", 40);
    if (!font) {
        SDL_Log("- Failed to load font slkscr.ttf: %s", SDL_GetError());
        return -1;
    }

    // sprites init
    SDL_Surface* bg_s = IMG_Load("assets/desert_pixel.bmp");
    if(bg_s == NULL){
        SDL_Log("- IMG_Load failed: %s", SDL_GetError());
        return -1;
    }
    SDL_Surface* sprites_s = IMG_Load("assets/base64.bmp");
    if(sprites_s == NULL){
        SDL_Log("- IMG_Load failed: %s", SDL_GetError());
        return -1;
    }
    bg_txt = SDL_CreateTextureFromSurface(renderer, bg_s);
    sprites_txt = SDL_CreateTextureFromSurface(renderer, sprites_s);

    SDL_DestroySurface(bg_s);
    SDL_DestroySurface(sprites_s);

    ground = w_bounds.h - w_bounds.h / 10;
    dino->mid_air = false;
    dino->long_jump = false;
    dino->ducking = false;
    dino->reward = 0;
    dino->hitbox = (SDL_FRect){ w_bounds.w / 8, ground - DINO_H, DINO_W, DINO_H};

    Level_Init();

    return 0;
}

int DR_Start(){

    done = false;
    dead = false;
    int feedback = 0;
    high_score = 0;

    if(DR_Init() != 0){
        SDL_Log("- couldnt init Game!\n");
        return -1;
    }

    // high score file init
    FILE* high_score_file = NULL;

    fopen_s(&high_score_file, "high_score.txt", "r");

    if(high_score_file == NULL){
        SDL_Log("- couldnt open high score file to read high score!\n");
        return -1;
    }

    char b[128];
    fgets(b, 100, high_score_file);
    total_high_score = atoi(b);
    fclose(high_score_file);
       //SDL_Log("+ high score %d\n", total_high_score);
    feedback = DR_Run();
    if(feedback == GAME_WINDOW_EVENT_EXIT_CRASH){
        SDL_Log("- Unexpected Game crash!\n");
    } 

    if(high_score > total_high_score){

        fopen_s(&high_score_file, "high_score.txt", "w");

         if(high_score_file == NULL){
            SDL_Log("- couldnt open high score file to store high score!\n");
            return -1;
        }
        // over write existing buffer
        snprintf(b, 100, "%d", high_score);
        fputs(b, high_score_file);
        fclose(high_score_file);
    }

    SDL_Log("----------------------------\n");
    SDL_Log("+ Cleanup epilouge\n");
    SDL_Log("----------------------------\n\n");

    if(!LL_Clean(obj_list)){
        SDL_Log("- couldnt properly free Linked List data structure\n");
    }else{
        SDL_Log("+ freed Linked List data structure\n");
    }

    free(dino);
    free(obj_list);
    SDL_Log("+ freed allocated structs\n");

    SDL_DestroyTexture(bg_txt);
    SDL_DestroyTexture(sprites_txt);

    SDL_Log("+ cleaned textures/surfaces\n");

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_Log("+ disposed SDL3_TTF\n");

    return feedback;
}

int DR_Run(){
  
    int feedback = DR_GameLoop();
    if(feedback == GAME_WINDOW_EVENT_EXIT_CRASH){
        SDL_Log("- something went wrong inside GameLoop!\n");
        return GAME_WINDOW_EVENT_EXIT_CRASH;
    }

    return feedback;
}


int DR_GameLoop(){

 
    int window_feedback = 0;

    while (!done) {
        
        DR_GameLogic();
        GameWindowEvent gwe = {0, 0};
        window_feedback = WindowEvents(&gwe);

        DR_ShowFPS();
        SDL_RenderPresent(renderer);
        DR_LoopThrottle();

    }

    return window_feedback;
}



void DR_LoopThrottle(){

    static Uint64 target = 0;
    static double interval = 1.f / DESIRED_FPS;  
    interval = 1.f / actual_fps;

    Uint64 now = SDL_GetPerformanceCounter();
    //if loop cycle went too fast(counter is smaller than previous defined target delta time) for one interval -> sleep for the remainder time
    while (now < target) {
        // call SDL_Delay(0) to yield the thread
        SDL_Delay(0);
        now = SDL_GetPerformanceCounter();
    }  
    target = now + (Uint64)(interval * SDL_GetPerformanceFrequency()); 

}

void DR_GameLogic(){

    if(!dead){
        Dino_Move();
        dead = Level_Update();
    }

    DR_Draw();
}

void DR_RenderText(int x, int y, SDL_Color col, TextLayout layout, SDL_Surface* text_sur){

    int padding = 10, x_adjust = 0, y_adjust = 0;
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_sur);

    //by default sdl assumes you want fast, opaque rendering -> unless explicitly spcified otherwise.
    SDL_SetTextureAlphaMod(text_texture, col.a); 

    if(layout & TOP)
        y_adjust = -text_sur->h;
    if(layout & BOTTOM)
        y_adjust = text_sur->h;

    if(layout & RIGHT)
        x_adjust = -text_sur->w - padding*2;

    if(layout & CENTER){
        x_adjust = -text_sur->w/2;
        y_adjust = -text_sur->h/2;
    }
    if(layout & CENTER_Y)
        y_adjust = -text_sur->h/2;
    if(layout & CENTER_X)
        x_adjust = -text_sur->w/2;
    

    SDL_FRect dst = {x + x_adjust + padding, y + y_adjust + padding, text_sur->w, text_sur->h}; 
    SDL_RenderTexture(renderer, text_texture, NULL, &dst); 
    SDL_DestroyTexture(text_texture);
}

Bounds DR_DrawText(int x, int y, TextLayout layout, SDL_Color col, char* buff){

    SDL_Surface* text_sur = TTF_RenderText_Blended(font, buff, 0, col);
    
    if (!text_sur) 
        return (Bounds){0, 0}; 
    
    DR_RenderText(x, y, col, layout, text_sur);

    Bounds b = (Bounds){text_sur->w, text_sur->h};
    SDL_DestroySurface(text_sur);

    return b;
}

void DR_Draw(){

    // background
    SDL_RenderTexture(renderer, bg_txt, NULL, NULL);

    char buff[128];

    snprintf(buff, sizeof(buff), "HI: %d", high_score);
    int text_width = w_bounds.w - DR_DrawText(w_bounds.w, 0, RIGHT, (SDL_Color) {0, 0, 0, 255} ,buff).w - 20;

    snprintf(buff, sizeof(buff), "%d", total_high_score);
    text_width -= DR_DrawText(text_width, 0, RIGHT, (SDL_Color) {0xff, 0x33, 0x00, 0xff} ,buff).w;

    snprintf(buff, sizeof(buff), "TOTAL HI: ");
    DR_DrawText(text_width, 0, RIGHT, (SDL_Color) {0, 0, 0, 255} ,"TOTAL HI: ");

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    Dino_Draw();

    if(obj_list->size > 0){
        Node* curr = obj_list->NIL;
        while((curr = curr->next) != obj_list->NIL){
            ((Obstacle*)curr->data)->draw((Obstacle*)curr->data);
        }
    }
    
    // if dead render all current entities with dead screen on top
    if(dead){
        static float retry_animation_counter = 0;
        static int retry_animation = 0;
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 50);
        SDL_FRect screen = (SDL_FRect){0,0,w_bounds.w, w_bounds.h};
        SDL_RenderFillRect(renderer, &screen);

        retry_animation_counter += 160.f/actual_fps;
        if(retry_animation_counter > actual_fps / 3){
            retry_animation = (retry_animation + 1) % 4;
            retry_animation_counter = 0;
        }
            
        SDL_FRect retry_r_src = { 145 + retry_animation * 72, 130 , 72 , 70 };
        SDL_FRect retry_r = { w_bounds.w / 2 - 50 , w_bounds.h / 2 - 50, 72, 70 };
        SDL_RenderTexture(renderer, sprites_txt, &retry_r_src, &retry_r);

       // SDL_RenderRect(renderer,& retry_r);
    }
}



//wrapper
uint64_t millis(){

#ifdef WIN
    return GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); 
    return ts.tv_nsec;
#endif
}