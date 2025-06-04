cmake_minimum_required(VERSION 3.14)
project(DinoRush)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

# Main game executable
set(GAME_SOURCES
    ../src/main.c
    ../src/game_object.c
    ../src/level.c
    ../src/window.c
    ../src/ll.c
    ../src/dino_rush.c
)

add_executable(DinoRush ${GAME_SOURCES})

target_include_directories(DinoRush PRIVATE ../header)

target_link_libraries(DinoRush 
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    m
)

# Testing setup
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # Download and configure GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    enable_testing()
    
    # Test executable - compile C files as C, not C++
    add_executable(test_game
        ../test/test_game.cpp
    )
    
    # Create a static library from C sources for tests
    add_library(game_lib STATIC
        ../src/game_object.c
        ../src/level.c
        ../src/window.c
        ../src/ll.c
        ../src/dino_rush.c
    )
    
    target_include_directories(game_lib PUBLIC ../header)
    target_link_libraries(game_lib 
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        m
    )
    
    target_include_directories(test_game PRIVATE ../header)
    
    target_link_libraries(test_game
        game_lib
        gtest
        gtest_main
        gmock
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        m
    )
    
    # Add test discovery
    include(GoogleTest)
    gtest_discover_tests(test_game)
endif()

# Copy assets to build directory
file(COPY ../assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})