## CMake config for FoxGLBox_TestBed

cmake_minimum_required(VERSION 3.10)

project(FoxGLBox_TestBed)

set(BED_SOURCES
    Main.cpp)

source_group("Source/" FILES ${BED_SOURCES})

## FoxGLBox_TestBed.exe
add_executable(FoxGLBox_TestBed
        ${BED_SOURCES})

## Output directory
set_target_properties(FoxGLBox_TestBed PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin
    FOLDER Samples)

## Includes
target_include_directories(FoxGLBox_TestBed PUBLIC
    ${PROJECT_SOURCE_DIR}/../../extern/glm
    ${PROJECT_SOURCE_DIR}/../../extern/SDL2-2.0.14/include
    ${PROJECT_SOURCE_DIR}/../../renderer/public)

## Notify the linker
target_link_libraries(FoxGLBox_TestBed
        FoxGLBox
        ${PROJECT_SOURCE_DIR}/../../extern/SDL2-2.0.14/lib/x64/SDL2.lib
        ${PROJECT_SOURCE_DIR}/../../extern/SDL2-2.0.14/lib/x64/SDL2main.lib)

