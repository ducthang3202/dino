#include "../header/game_object.h"

void Dino_Draw(GameObject* go){

    Dino* dino = (Dino*)go->childclass_ref;

    static int dino_animation_frame_counter = 0;
    static bool dino_running_animation = true;

    int ducking_offset_y = 0;
    if(dino_animation_frame_counter < actual_fps / 4){
        dino_animation_frame_counter++;
    }else{
        //printf("anim %d\n", dino_running_animation);
        dino_running_animation = !dino_running_animation;

        dino_animation_frame_counter = 0;
    }

    int sprite_offset = go->hitbox.w * ((dino_running_animation + 2) * !dino->mid_air)  * !dino->dead + dino->dead * 5 * go->hitbox.w;
    if(dino->ducking){
        ducking_offset_y = 35;
        sprite_offset = 527 + 118 * dino_running_animation;
        go->hitbox.y = GROUND - DINO_H + 30;
        go->hitbox.h = 60;
        go->hitbox.w = 118;

    }else if(!dino->mid_air){
        go->hitbox.y = GROUND - DINO_H;
        go->hitbox.h = DINO_H;
        go->hitbox.w = DINO_W;
    }
    
    // mathematic equation is based on the order of sprites in base64.bmp
    // dino_on_ground is either true or false, anulling walking animation when mid air
    // if dino is dead (dead = 1) anull all other animation while index dead dino sprite (dead * 5)

    SDL_FRect dino_r_src = { 1678 + sprite_offset, ducking_offset_y , go->hitbox.w , go->hitbox.h };
    SDL_RenderTexture(renderer, go->sprites_txt, &dino_r_src, &go->hitbox);
  //  SDL_Log("%d\n", 1678 + sprite_offset);
    if(!show_hitbox)
        return;
    //
    SDL_RenderRect(renderer, &go->hitbox);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_FRect grace_box = (SDL_FRect){go->hitbox.x + GRACE_ZONE, go->hitbox.y + GRACE_ZONE / 2, go->hitbox.w - GRACE_ZONE * 2, go->hitbox.h - GRACE_ZONE * 1.5f };
    SDL_RenderRect(renderer, &grace_box);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void Obstacle_DrawCactus(GameObject* go){

    Obstacle* self = (Obstacle*)go->childclass_ref;
    // sprite optimization
    int g = 0;
    if(self->sprite > 2)
        g = 2;
    SDL_FRect cactus_r_src = { 653 + CACTUS_W * self->sprite + g, 0 , self->go.hitbox.w , self->go.hitbox.h };
    SDL_RenderTexture(renderer, go->sprites_txt, &cactus_r_src, &self->go.hitbox);

    if(!show_hitbox)
        return;

    SDL_RenderRect(renderer, &self->go.hitbox);
}
void Obstacle_DrawPterodactyl(GameObject* go){

    Obstacle* self = (Obstacle*)go->childclass_ref;
    self->animation_counter++;

    if(self->animation_counter > actual_fps/2){
        self->animation_counter = 0;
        self->sprite = (self->sprite + 1) % 2;
    }

    SDL_FRect pt_r_src = { 260 + 90 * self->sprite, 0 , 90, 85 };
    SDL_FRect pt_r = { go->hitbox.x, go->hitbox.y , 90, 85 };
    SDL_RenderTexture(renderer, go->sprites_txt, &pt_r_src, &pt_r);

    if(!show_hitbox)
        return;
        
    SDL_RenderRect(renderer, &pt_r);
}
void Dino_Move(GameObject* go){

    Dino* dino = (Dino*)go->childclass_ref;
    dino->go.hitbox.y = dino->ducking ? GROUND - DINO_H + 30 : GROUND - DINO_H;

    if(dino->jump_angle != 0 && dino->jump_angle < PI){

        float high_jump = GAME_WINDOW_HEIGHT / 3 + GAME_WINDOW_HEIGHT / 10;
        float low_jump = GAME_WINDOW_HEIGHT / 6;

        int amplitude = 0;
        if(dino->long_jump){
          //  printf("high jump: \n");
            amplitude = high_jump;
        }else{
            amplitude = low_jump;
        }
        
        dino->go.hitbox.y -= sin(dino->jump_angle) * amplitude;
        dino->jump_angle += PI/(actual_fps);
        dino->mid_air = true;///
    }

    if(dino->jump_angle >= PI){
        dino->jump_angle = 0;
        dino->long_jump = false;
        dino->mid_air = false;

        // collect reward
        if(dino->reward != 0){
            high_score += dino->reward;
            dino->reward = 0;
        } 
    }
}

void Dino_PassedObstacle(Dino* dino, Obstacle* obs){
    if((dino->go.hitbox.x + GRACE_ZONE > obs->go.hitbox.x && dino->go.hitbox.x + GRACE_ZONE < obs->go.hitbox.x + obs->go.hitbox.w ||
        dino->go.hitbox.x + dino->go.hitbox.w - GRACE_ZONE > obs->go.hitbox.x && dino->go.hitbox.x + dino->go.hitbox.w - GRACE_ZONE < obs->go.hitbox.x + obs->go.hitbox.w)){
            if(!dino->long_jump)
                dino->reward = 20;
            else if(obs->type == AIR)
                dino->reward = 30;
            else
                dino->reward = 10;
        }

}

void Obstacle_MoveCactus(GameObject* go){

    Obstacle* self = (Obstacle*)go->childclass_ref;
    self->go.hitbox.x -= 244.f/actual_fps;
}
void Obstacle_MovePterodactyl(GameObject* go){

    Obstacle* self = (Obstacle*)go->childclass_ref;
    self->go.hitbox.x -= 304.f/actual_fps;
}
