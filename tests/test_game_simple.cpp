#include <iostream>
#include <cassert>
#include <cmath>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

extern "C" {
    #include "../header/dino_rush.h"
    #include "../header/game_object.h"
    #include "../header/level.h"
    #include "../header/window.h"
    #include "../header/ll.h"
}

// Simple test framework
int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        std::cout << "[ RUN      ] " << #name << std::endl; \
        tests_run++; \
        try { \
            test_##name(); \
            tests_passed++; \
            std::cout << "[       OK ] " << #name << std::endl; \
        } catch (std::exception& e) { \
            tests_failed++; \
            std::cout << "[  FAILED  ] " << #name << " - " << e.what() << std::endl; \
        } \
    } \
    void test_##name()

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error("ASSERT_TRUE failed: " #condition); \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        throw std::runtime_error("ASSERT_FALSE failed: " #condition); \
    }

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        throw std::runtime_error("ASSERT_EQ failed: " #expected " != " #actual); \
    }

#define ASSERT_NE(expected, actual) \
    if ((expected) == (actual)) { \
        throw std::runtime_error("ASSERT_NE failed: " #expected " == " #actual); \
    }

#define ASSERT_GT(val1, val2) \
    if (!((val1) > (val2))) { \
        throw std::runtime_error("ASSERT_GT failed: " #val1 " <= " #val2); \
    }

#define ASSERT_LT(val1, val2) \
    if (!((val1) < (val2))) { \
        throw std::runtime_error("ASSERT_LT failed: " #val1 " >= " #val2); \
    }

#define ASSERT_NEAR(val1, val2, tolerance) \
    if (std::abs((val1) - (val2)) > (tolerance)) { \
        throw std::runtime_error("ASSERT_NEAR failed: difference exceeds tolerance"); \
    }

#define RUN_TEST(name) run_test_##name()

// Tests
TEST(LinkedList_CreateAndDestroy) {
    LinkedList* list = LL_Create();
    ASSERT_NE(list, nullptr);
    ASSERT_EQ(list->size, 0);
    
    LL_Clean(list);
    free(list);
}

TEST(LinkedList_AddAndRemove) {
    LinkedList* list = LL_Create();
    
    int* data1 = (int*)malloc(sizeof(int));
    *data1 = 42;
    LL_Add(list, data1);
    ASSERT_EQ(list->size, 1);
    
    int* data2 = (int*)malloc(sizeof(int));
    *data2 = 100;
    LL_Add(list, data2);
    ASSERT_EQ(list->size, 2);
    
    LL_Remove(list, data1);
    ASSERT_EQ(list->size, 1);
    
    LL_Remove(list, data2);
    ASSERT_EQ(list->size, 0);
    
    LL_Clean(list);
    free(list);
}

TEST(Dino_Initialization) {
    Dino dino;
    dino.mid_air = false;
    dino.long_jump = false;
    dino.ducking = false;
    dino.dead = false;
    dino.reward = 0;
    dino.jump_angle = 0;
    
    ASSERT_FALSE(dino.mid_air);
    ASSERT_FALSE(dino.long_jump);
    ASSERT_FALSE(dino.ducking);
    ASSERT_FALSE(dino.dead);
    ASSERT_EQ(dino.reward, 0);
    ASSERT_EQ(dino.jump_angle, 0);
}

TEST(Dino_Movement) {
    SDL_Window* window = SDL_CreateWindow("Test", 1, 1, SDL_WINDOW_HIDDEN);
    renderer = SDL_CreateRenderer(window, NULL);
    
    Dino dino;
    dino.go.hitbox = (SDL_FRect){100, GROUND_HEIGHT - DINO_H, DINO_W, DINO_H};
    dino.go.move = Dino_Move;
    dino.go.childclass_ref = &dino;
    dino.mid_air = false;
    dino.long_jump = false;
    dino.ducking = false;
    dino.dead = false;
    dino.reward = 0;
    dino.jump_angle = 0;
    
    float initial_y = dino.go.hitbox.y;
    
    // Start a jump
    dino.jump_angle = M_PI / 60;
    dino.mid_air = true;
    
    // Move once
    Dino_Move(&dino.go);
    
    // Y position should have changed
    ASSERT_NE(dino.go.hitbox.y, initial_y);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = nullptr;
}

TEST(Obstacle_Structure) {
    Obstacle obs;
    obs.type = GROUND;
    obs.sprite = 0;
    obs.animation_counter = 0;
    obs.go.hitbox = (SDL_FRect){1000, 400, 50, 100};
    
    ASSERT_EQ(obs.type, GROUND);
    ASSERT_EQ(obs.sprite, 0);
    ASSERT_EQ(obs.go.hitbox.x, 1000);
}

TEST(Obstacle_Movement) {
    Obstacle cactus;
    cactus.go.hitbox = (SDL_FRect){1000, GROUND_HEIGHT - CACTUS_H, CACTUS_W, CACTUS_H};
    cactus.go.move = Obstacle_MoveCactus;
    cactus.go.childclass_ref = &cactus;
    
    float initial_x = cactus.go.hitbox.x;
    actual_fps = 60;
    
    cactus.go.move(&cactus.go);
    
    ASSERT_LT(cactus.go.hitbox.x, initial_x);
}

TEST(Game_HighScore) {
    high_score = 0;
    
    high_score += 10;
    ASSERT_EQ(high_score, 10);
    
    high_score += 20;
    ASSERT_EQ(high_score, 30);
}

TEST(Window_Dimensions) {
    ASSERT_GT(GAME_WINDOW_WIDTH, 0);
    ASSERT_GT(GAME_WINDOW_HEIGHT, 0);
    ASSERT_GT(GROUND_HEIGHT, 0);
    ASSERT_LT(GROUND_HEIGHT, GAME_WINDOW_HEIGHT);
}

TEST(Collision_BasicRect) {
    SDL_FRect rect1 = {100, 100, 50, 50};
    SDL_FRect rect2 = {200, 100, 50, 50};
    
    // No collision when apart
    bool collides = (rect1.x < rect2.x + rect2.w &&
                    rect1.x + rect1.w > rect2.x &&
                    rect1.y < rect2.y + rect2.h &&
                    rect1.y + rect1.h > rect2.y);
    ASSERT_FALSE(collides);
    
    // Collision when overlapping
    rect2.x = 120;
    collides = (rect1.x < rect2.x + rect2.w &&
               rect1.x + rect1.w > rect2.x &&
               rect1.y < rect2.y + rect2.h &&
               rect1.y + rect1.h > rect2.y);
    ASSERT_TRUE(collides);
}

TEST(Level_Update) {
    SDL_Window* window = SDL_CreateWindow("Test", 1, 1, SDL_WINDOW_HIDDEN);
    renderer = SDL_CreateRenderer(window, NULL);
    
    LinkedList* obstacles = LL_Create();
    Dino dino;
    dino.dead = false;
    dino.mid_air = false;
    dino.reward = 0;
    dino.go.hitbox = (SDL_FRect){100, GROUND_HEIGHT - DINO_H, DINO_W, DINO_H};
    dino.go.sprites_txt = nullptr;
    
    // Should not crash
    Level_Update(obstacles, &dino);
    
    // Clean up
    while (obstacles->size > 0) {
        Node* node = obstacles->NIL->next;
        if (node != obstacles->NIL) {
            LL_Remove(obstacles, node->data);
        }
    }
    
    LL_Clean(obstacles);
    free(obstacles);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = nullptr;
}

// Main test runner
int main(int argc, char** argv) {
    (void)argc; (void)argv; // Suppress unused warnings
    
    std::cout << "[==========] Running tests" << std::endl;
    
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    if (!TTF_Init()) {
        std::cerr << "Failed to initialize SDL_ttf: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    // Initialize globals
    high_score = 0;
    show_hitbox = false;
    actual_fps = 60.0f;
    
    // Run all tests
    RUN_TEST(LinkedList_CreateAndDestroy);
    RUN_TEST(LinkedList_AddAndRemove);
    RUN_TEST(Dino_Initialization);
    RUN_TEST(Dino_Movement);
    RUN_TEST(Obstacle_Structure);
    RUN_TEST(Obstacle_Movement);
    RUN_TEST(Game_HighScore);
    RUN_TEST(Window_Dimensions);
    RUN_TEST(Collision_BasicRect);
    RUN_TEST(Level_Update);
    
    // Print summary
    std::cout << "[==========] " << tests_run << " tests ran" << std::endl;
    if (tests_failed == 0) {
        std::cout << "[  PASSED  ] " << tests_passed << " tests" << std::endl;
    } else {
        std::cout << "[  PASSED  ] " << tests_passed << " tests" << std::endl;
        std::cout << "[  FAILED  ] " << tests_failed << " tests" << std::endl;
    }
    
    // Cleanup
    TTF_Quit();
    SDL_Quit();
    
    return tests_failed > 0 ? 1 : 0;
}