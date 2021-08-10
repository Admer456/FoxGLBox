## Top-level CMake config for FoxGLBox

cmake_minimum_required(VERSION 3.10)

## Renderer project
project(FoxGLBox)

## Some property stuff
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)

## Options
option(FOX_BUILD_SAMPLES
        "Build samples" OFF)
option(FOX_USE_ASSIMP
        "Build and use Assimp instead of the built-in OBJ parser" OFF)

## Add the renderer
add_subdirectory(renderer)

## Add samples
if (FOX_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
