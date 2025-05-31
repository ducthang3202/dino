#pragma once

#include <time.h>
#include <stdlib.h>

#define MAX_OBJECTS 10

// grace zone for player hitbox collision
#define GRACE_ZONE 28

#include "game_object.h"
#include "ll.h"

extern uint64_t millis();

void Level_Update(LinkedList* obj_list, Dino* dino);
void Level_Init(LinkedList* obj_list);
int r(int min, int max);