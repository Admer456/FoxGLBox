## CMake config for FoxGLBox_TestGame

cmake_minimum_required(VERSION 3.10)

project(FoxGLBox_TestGame)

set(CMAKE_CXX_STANDARD 17)

set(GAME_INCLUDES
    Engine.hpp
    GameEntity.hpp
    IEngine.hpp
    Vector.hpp)

set(GAME_SOURCES
    Engine.cpp
    GameEntity.cpp
    Main.cpp
    Vector.cpp)

source_group("Source/" FILES ${GAME_INCLUDES})
source_group("Source/" FILES ${GAME_SOURCES})

## FoxGLBox_TestGame.exe
add_executable(FoxGLBox_TestGame
        ${GAME_INCLUDES}
        ${GAME_SOURCES})

## Output directory
set_target_properties(FoxGLBox_TestGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin
    FOLDER Samples)

## Includes
target_include_directories(FoxGLBox_TestGame PUBLIC
    ${PROJECT_SOURCE_DIR}/../../extern/glm
    ${PROJECT_SOURCE_DIR}/../../extern/SDL2-2.0.14/include
    ${PROJECT_SOURCE_DIR}/../../renderer/public)

## Notify the linker
target_link_libraries(FoxGLBox_TestGame
        FoxGLBox
        ${PROJECT_SOURCE_DIR}/../../extern/SDL2-2.0.14/lib/x64/SDL2.lib
        ${PROJECT_SOURCE_DIR}/../../extern/SDL2-2.0.14/lib/x64/SDL2main.lib)

