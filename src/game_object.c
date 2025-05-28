#include "../header/game_object.h"


int dino_animation_frame_counter = 0;
bool dino_running_animation = true;
float angle = 0;

void Dino_Draw(){

    int ducking_offset_y = 0;
    if(dino_animation_frame_counter < FPS / 4){
        dino_animation_frame_counter++;
    }else{
        //printf("anim %d\n", dino_running_animation);
        dino_running_animation = !dino_running_animation;

        dino_animation_frame_counter = 0;
    }

    int sprite_offset = dino->hitbox.w * ((dino_running_animation + 2) * !dino->mid_air)  * !dead + dead * 5 * dino->hitbox.w;
    if(dino->ducking){
        ducking_offset_y = 35;
        sprite_offset = 527 + 118 * dino_running_animation;
        dino->hitbox.y = ground - DINO_H + 30;
        dino->hitbox.h = 60;
        dino->hitbox.w = 118;

    }else if(!dino->mid_air){
        dino->hitbox.y = ground - DINO_H;
        dino->hitbox.h = DINO_H;
        dino->hitbox.w = DINO_W;
    }
    
    // mathematic equation is based on the order of sprites in base64.bmp
    // dino_on_ground is either true or false, anulling walking animation when mid air
    // if dino is dead (dead = 1) anull all other animation while index dead dino sprite (dead * 5)

    SDL_FRect dino_r_src = { 1678 + sprite_offset, ducking_offset_y , dino->hitbox.w , dino->hitbox.h };
    SDL_RenderTexture(renderer, sprites_txt, &dino_r_src, &dino->hitbox);
  //  SDL_Log("%d\n", 1678 + sprite_offset);
    if(!show_hitbox)
        return;
    
    SDL_RenderRect(renderer, &dino->hitbox);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_FRect grace_box = (SDL_FRect){dino->hitbox.x + GRACE_ZONE, dino->hitbox.y + GRACE_ZONE / 2, dino->hitbox.w - GRACE_ZONE * 2, dino->hitbox.h - GRACE_ZONE * 1.5f };
    SDL_RenderRect(renderer, &grace_box);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void Obstacle_DrawCactus(Obstacle* self){

    // sprite optimization
    int g = 0;
    if(self->sprite > 2)
        g = 2;
    SDL_FRect cactus_r_src = { 653 + CACTUS_W * self->sprite + g, 0 , self->hitbox.w , self->hitbox.h };
    SDL_RenderTexture(renderer, sprites_txt, &cactus_r_src, &self->hitbox);

    if(!show_hitbox)
        return;

    SDL_RenderRect(renderer, &self->hitbox);
}
void Obstacle_DrawPterodactyl(Obstacle* self){

    self->animation_counter++;
    if(self->animation_counter > FPS/2){
        self->animation_counter = 0;
        self->sprite = (self->sprite + 1) % 2;
    }
    SDL_FRect pt_r_src = { 260 + 90 * self->sprite, 0 , 90, 85 };
    SDL_FRect pt_r = { self->hitbox.x, self->hitbox.y , 90, 85 };
    SDL_RenderTexture(renderer, sprites_txt, &pt_r_src, &pt_r);

    if(!show_hitbox)
        return;
        
    SDL_RenderRect(renderer, &pt_r);
}
void Dino_Move(){
    dino->hitbox.y = dino->ducking ? ground - DINO_H + 30 : ground - DINO_H;

    if(angle != 0 && angle < PI){

        float high_jump = w_bounds.h / 3 + w_bounds.h / 10;
        float low_jump = w_bounds.h / 6;

        int amplitude = 0;
        if(dino->long_jump){
            printf("high jump: \n");
            amplitude = high_jump;
        }else{
            amplitude = low_jump;
        }
        
        dino->hitbox.y -= sin(angle) * amplitude;
        angle += PI/((float)FPS*1.1f);
        dino->mid_air = true;
    }

    if(angle >= PI){
        angle = 0;
        dino->long_jump = false;
        dino->mid_air = false;

        // collect reward
        if(dino->reward != 0){
            high_score += dino->reward;
            dino->reward = 0;
        } 
    }
}

void Dino_PassedObstacle(Obstacle* obs){
    if((dino->hitbox.x + GRACE_ZONE > obs->hitbox.x && dino->hitbox.x + GRACE_ZONE < obs->hitbox.x + obs->hitbox.w ||
        dino->hitbox.x + dino->hitbox.w - GRACE_ZONE > obs->hitbox.x && dino->hitbox.x + dino->hitbox.w - GRACE_ZONE < obs->hitbox.x + obs->hitbox.w)){
            if(!dino->long_jump)
                dino->reward = 20;
            else if(obs->type == AIR)
                dino->reward = 30;
            else
                dino->reward = 10;
        }

}

void Obstacle_MoveCactus(Obstacle* self){
    self->hitbox.x -= 1.7f;
}
void Obstacle_MovePterodactyl(Obstacle* self){
    self->hitbox.x -= 2.2f;
}
