## CMake config for FoxGLBox

cmake_minimum_required(VERSION 3.10)

## =======================================================
## Source files
## =======================================================
## renderer/public/
set(FGL_PUBLIC_INCLUDES
    public/DrawGeometry.hpp
    public/IMaterial.hpp
    public/IRenderWorld.hpp
    public/RenderEntityParams.hpp
    public/RenderModelParams.hpp
    public/RenderView.hpp)

## renderer/src/
set(FGL_INCLUDES
    src/FrontendTexture.hpp
    src/IRenderer.hpp
    src/Material.hpp
    src/Model.hpp
    src/RenderEntity.hpp
    src/RenderWorld.hpp)

set(FGL_SOURCES
    src/FrontendTexture.cpp
    src/Material.cpp
    src/Model.cpp
    src/RenderSystem.cpp
    src/RenderWorld.cpp)

## renderer/src/Backends/
set(FGL_BACKENDS_INCLUDES
    src/Backends/BackendRegistry.hpp)

set(FGL_BACKENDS_SOURCES
    src/Backends/BackendRegistry.cpp)

## renderer/src/Backends/OpenGL45/
set(FGL_BACKENDS_GL45_INCLUDES
    src/Backends/OpenGL45/Renderer.hpp
    src/Backends/OpenGL45/Shader.hpp
    src/Backends/OpenGL45/Texture.hpp
    src/Backends/OpenGL45/VertexBuffer.hpp)
    
set(FGL_BACKENDS_GL45_SOURCES
    src/Backends/OpenGL45/Renderer.cpp
    src/Backends/OpenGL45/Shader.cpp
    src/Backends/OpenGL45/Texture.cpp
    src/Backends/OpenGL45/VertexBuffer.cpp)

## =======================================================
## Folder organisation
## =======================================================
source_group("Public/" FILES ${FGL_PUBLIC_INCLUDES})
source_group("Source/" FILES ${FGL_INCLUDES})
source_group("Source/" FILES ${FGL_SOURCES})
source_group("Source/Backends/" FILES ${FGL_BACKENDS_INCLUDES})
source_group("Source/Backends/" FILES ${FGL_BACKENDS_SOURCES})
source_group("Source/Backends/OpenGL45/" FILES ${FGL_BACKENDS_GL45_INCLUDES})
source_group("Source/Backends/OpenGL45/" FILES ${FGL_BACKENDS_GL45_SOURCES})

## FoxGLBox.lib
add_library(FoxGLBox 
        ${FGL_PUBLIC_INCLUDES} 
        ${FGL_INCLUDES} 
        ${FGL_SOURCES}
        ${FGL_BACKENDS_INCLUDES}
        ${FGL_BACKENDS_SOURCES}
        ${FGL_BACKENDS_GL45_INCLUDES}
        ${FGL_BACKENDS_GL45_SOURCES})

## Include directories
set(FGL_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/extern/glew-2.1.0/include
        ${PROJECT_SOURCE_DIR}/extern/glm   
        ${PROJECT_SOURCE_DIR}/extern/stb-image   
        ${PROJECT_SOURCE_DIR}/renderer/public
        ${PROJECT_SOURCE_DIR}/renderer/src)

## Linker libraries
set(FGL_LINK_LIBRARIES
    opengl32
    ${PROJECT_SOURCE_DIR}/extern/glew-2.1.0/lib/glew32s.lib)

## =======================================================
## Assimp stuff
## TODO: develop a plugin system and have Assimp as a plugin
## =======================================================
if (FOX_USE_ASSIMP)
    
    add_definitions(-DFOX_USE_ASSIMP)

    set(FGL_INCLUDE_DIRECTORIES
        ${FGL_INCLUDE_DIRECTORIES}
        ${PROJECT_SOURCE_DIR}/extern/assimp/include)

    set(FGL_LINK_LIBRARIES
        ${FGL_LINK_LIBRARIES}
        ${PROJECT_SOURCE_DIR}/extern/assimp/lib/assimp-vc142-mt.lib)

else()

    add_definitions(-DFOX_USE_INTERNAL_OBJ_LOADER)

endif()

## Set output directory and stuff
set_target_properties(FoxGLBox PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
    FOLDER Libraries)

## Include necessary stuff
target_include_directories(FoxGLBox PUBLIC
    ${FGL_INCLUDE_DIRECTORIES})

## Linker input
target_link_libraries(FoxGLBox
    ${FGL_LINK_LIBRARIES})
