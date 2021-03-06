#include "IEngine.hpp"
#include "Vector.hpp"
#include "IRenderWorld.hpp"
#include "GameEntity.hpp"

#include <iostream>
#include <chrono>
#include "SDL.h"
#include "SDL_opengl.h"
#include "glm/gtc/matrix_transform.hpp"

#include "Engine.hpp"

namespace chrono = std::chrono;

IEngine* IEngine::AllocateInstance()
{
    return new Engine();
}

void Engine::Init( const char* title, int width, int height )
{
    gEngine = this;

    // Init SDL and create a window
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );

    // Set the OpenGL context version
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
    
    // Create the window and context
	window = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL );
	SDL_GLContext glContext = SDL_GL_CreateContext( window );

    // Initialise the render system with render init params
    renderSystem = foxglbox::GetRenderSystem();
    RenderInitParams rip {
        width, height,                      // Resolution
        RenderInitParams::Renderer_OpenGL45,// OpenGL 4.5 render backend
        RenderInitParams::Windowing_SDL2,   // SDL2 windowing
        glContext                           // OpenGL 4.5 context
    };
    renderWorld = renderSystem->InitRenderer( rip );

    windowWidth = width;
    windowHeight = height;

    // Turning on VSync here so my GPU (and yours) doesn't crash'n'burn
	SDL_GL_SetSwapInterval( 0 );
    // Set relative mouse mode
    SDL_SetRelativeMouseMode( SDL_TRUE );

    // Populate the world with entities
    CreateGameEntities();
}

bool Engine::RunFrame()
{
    auto startPoint = chrono::system_clock::now();
    {
        SDL_Event event;
        while ( SDL_PollEvent( &event ) )
        {
            if ( event.type == SDL_QUIT )
            {
                return false;
            }

            if ( event.type == SDL_KEYDOWN )
            {
                if ( event.key.keysym.sym == SDLK_ESCAPE )
                {
                    return false;
                }
            }
        }

        // Update all updateable entities
        for ( auto& ent : gameEntities )
        {
            if ( nullptr != ent )
            {
                if ( ent->entityFlags.canThink )
                {
                    ent->Update( frameTime );
                }
            }
        }
        // Present the entities to the user (i.e. just update render entities)
        for ( auto& ent : gameEntities )
        {
            if ( nullptr != ent )
            {
                if ( ent->entityFlags.visible )
                {
                    ent->Present();
                }
            }
        }
        // Render the frame!
        renderWorld->RenderFrame( mainView );
        SDL_GL_SwapWindow( window );
    }
    auto endPoint = chrono::system_clock::now();
    auto microSeconds = chrono::duration_cast<chrono::microseconds>(endPoint - startPoint);

    frameTime = (microSeconds.count() / (1000.0f * 1000.0f)) * timeScale;
    float frameRate = 1.0f / frameTime;

    printf( "## Frame time: %3.2f ms\n## Frame rate: %4.1f fps\n\n", (frameTime * 1000.0f), frameRate );

    return true;
}

void Engine::Shutdown()
{
    for ( auto& ent : gameEntities )
    {
        if ( nullptr != ent )
        {
            delete ent;
            ent = nullptr;
        }
    }

    if ( nullptr != renderWorld )
    {
        renderWorld->Shutdown();
        delete renderWorld;
        renderWorld = nullptr;
    }
}

void Engine::Print( const char* string )
{
    std::cout << string;
}

RenderModelHandle Engine::GetModel( const char* modelPath )
{
    if ( nullptr == modelPath )
    {
        return RenderHandleInvalid;
    }

    RenderModelParams modelParams{ modelPath };
    return renderWorld->CreateModel( modelParams );
}

void Engine::SubmitRenderView( const RenderView& view, bool isMain )
{
    if ( isMain )
    {
        mainView = view;
        return;
    }
}

void Engine::CreateGameEntities()
{
    using namespace Entities;
    for ( auto& ent : gameEntities )
    {
        ent = nullptr;
    }

    RenderModelHandle terrainHandle = GetModel( "terrain.obj" );
    RenderModelHandle amanHandle = GetModel( "aman.obj" ); // The A-Man
    RenderModelHandle testCoverHandle = GetModel( "test_cover.obj" );
    RenderModelHandle testRockHandle = GetModel( "testrock.obj" );

    // Ideally, you'd wanna load some sorta level/scene file, but I decided to keep the sample very simple
    CreateEntity<Prop>( fglVector::Zero, fglVector( 90.0f, 0.0f, 0.0f ), terrainHandle );
    CreateEntity<Prop>( fglVector( 2.0f, 1.0f, 6.0f ), fglVector( 90.0f, 0.0f, 0.0f ), testCoverHandle );
    CreateEntity<Prop>( fglVector( 2.0f, 1.0f, 2.3f ), fglVector( 90.0f, 0.0f, 0.0f ), testRockHandle );
    CreateEntity<PropRotating>(  fglVector(  2.0f,  0.0f,  3.0f  ), fglVector::Zero, amanHandle );
    CreateEntity<PropInstanced>( fglVector(  4.0f, -2.0f,  5.0f  ), fglVector::Zero, testRockHandle );
    CreateEntity<Player>(        fglVector( -8.0f,  0.0f,  0.0f  ), fglVector::Zero, 0 );

    for ( auto& ent : gameEntities )
    {
        if ( nullptr != ent )
        {
            ent->Spawn();
        }
    }
}

template<typename EntityClass>
EntityClass* Engine::CreateEntity( fglVector position, fglVector angles, RenderModelHandle modelHandle )
{
    for ( auto& ent : gameEntities )
    {
        if ( nullptr == ent )
        {
            EntityClass* entity = Entities::GameEntity::Instance<EntityClass>( position, angles, modelHandle );
            ent = entity;
            return entity;
        }
    }

    return nullptr;
}

/*
Copyright (c) 2021 Admer456

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
