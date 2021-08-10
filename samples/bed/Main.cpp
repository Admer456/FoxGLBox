#include <iostream>
#include "SDL.h"
#include "SDL_opengl.h"
#include "IRenderWorld.hpp"

#include "glm/gtc/matrix_transform.hpp"

constexpr int Width = 1280;
constexpr int Height = 720;

/*

	Generally, the way this works is the following:
	* Create the window using your preferred framework (in this case SDL2)
	* Initialise the render system
	* Initialise the render world with render initialisation parameters (window resolution, preferred render backend etc.)
	* Create/load a model
	* Create a render entity with the model
	* Create a render view
	* Render

	In a game engine, it could go something like this:
	'''
	void Entity::Spawn()
	{
		renderEntity = renderWorld->CreateEntity( renderEntityParams );
	}

	void Entity::PrecacheModel( string modelPath )
	{
		renderEntityParams.model = renderWorld->CreateModel( { modelPath, RenderModelType::FromFile } );
	}
	'''
	oooor:
	'''
	void Game::Init()
	{
		if ( !IsDedicatedServer() )
		{	
			InitialiseRenderer();

			foreach ( modelFile in "assets/models" )
			{
				auto model = modelManager.LoadModel( modelFile );
				modelHandles.push_back( renderWorld->CreateModel( { model.GetPath(), RenderModelType::FromFile } );
			}
		}
		else 
		{
			foreach ( modelFile in "assets/models" )
			{
				modelManager.LoadModel( modelFile );
			}
		}
	}

	void Game::Draw()
	{
		renderWorld->RenderFrame( GetCurrentCamera().GetRenderView() );
	}
	'''

	You get the idea by now, I suppose.
*/

int main( int argc, char** argv )
{
	// =====================================
	// STEP 1
	// Init SDL2 here and make a window
	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );
	// Set the OpenGL context version
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
	// Create the window and context
	SDL_Window* window = SDL_CreateWindow( "FoxGLBox test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL );
	SDL_GLContext glContext = SDL_GL_CreateContext( window );
	
	// Also initialise the render system, which is just a thing 
	// that we can use to obtain other subsystems from FoxGLBox
	// 
	// At the moment, it only has the renderer system in it, but
	// later on, it'll also have the material system exposed
	//
	// GetRenderSystem doesn't allocate anything, so you can 
	// simply call foxglbox::GetRenderSystem() every time you wanna
	// access something. I just do this to decrease typing
	IRenderSystem* renderSystem = foxglbox::GetRenderSystem();

	// =====================================
	// STEP 2
	// Renderer initialisation parameters
	RenderInitParams rip {
		Width, Height,						// resolution
		RenderInitParams::Renderer_OpenGL45,// render backend
		RenderInitParams::Windowing_SDL2,	// SDL2 windowing
		glContext							// OpenGL context
	};

	// =====================================
	// STEP 3
	// Primary render view
	// You can think of a render view as a camera
	RenderView rv {
		Width, Height,					// viewport dimensions
		0,								// render mask
		glm::vec3( 0.0f, 0.0f, -1.0f ),	// position
		glm::identity<glm::mat4>(),		// orientation matrix
		105.0f,							// FOV
		false							// orthographic
	};

	// =====================================
	// STEP 4
	// Get the render world
	// The render world is basically the render "frontend" here
	IRenderWorld* renderWorld = renderSystem->InitRenderer( rip );
	if ( nullptr == renderWorld )
	{
		std::cout << "Could not initialise the renderer" << std::endl;
		return 1;
	}

	// =====================================
	// STEP 5
	// Load model
	RenderModelParams modelParams {
		"text2.obj",				// model path relative to the .exe
		RenderModelType::FromFile	// model type, dynamic models aren't yet there
	};

	// Actually load the model and get a handle
	auto modelHandle = renderWorld->CreateModel( modelParams );
	if ( modelHandle == RenderHandleInvalid )
	{
		std::cout << "Could not load text.obj";
		return 1;
	}

	// Create a render entity that uses this model
	RenderEntityParams entityParams {
		glm::vec3( 0.0f, 0.0f, 0.0f ),	// position
		glm::identity<glm::mat4>(),		// orientation
		modelHandle,					// model ID
		0								// render mask
	};

	// =====================================
	// STEP 6
	// Create the render entity and get a handle to it
	// Keep in mind, this is not a GAME entity, this is a RENDER entity
	auto entityHandle = renderWorld->CreateEntity( entityParams );
	if ( entityHandle == RenderHandleInvalid )
	{
		std::cout << "Could not create a render entity";
		return 1;
	}

	// Turning on VSync here so my GPU (and yours) doesn't crash'n'burn
	SDL_GL_SetSwapInterval( 1 );

	// =====================================
	// STEP 7
	// Render stuff
	float time = 0.0f;
	bool quit = false;
	SDL_Event e;

	while ( !quit )
	{
		time += 0.016f; // Update the time so we can rotate the render entities
		
		while ( SDL_PollEvent( &e ) )
		{
			if ( e.type == SDL_QUIT )
				quit = true;

			if ( e.type == SDL_KEYDOWN )
			{	// Press R to reload shaders
				if ( e.key.keysym.sym == SDLK_r )
					renderWorld->ReloadShaders();
				// Simple back'n'forth movement
				if ( e.key.keysym.sym == SDLK_w )
					rv.cameraPosition.z += 0.05f;

				if ( e.key.keysym.sym == SDLK_s )
					rv.cameraPosition.z -= 0.05f;
			}

			// Zooming in/out
			if ( e.type == SDL_MOUSEWHEEL )
				rv.cameraFov -= e.wheel.y * 5.0f;
		}

		// Example of moving the camera
		rv.cameraPosition.x = sin( time * 0.25f ) * 0.125f;

		// Crude example of rotating the render entity
		// Ideally you'd calculate/obtain the orientation matrix from your game's physics or directly from the game entity/object
		entityParams.orientation = glm::rotate( entityParams.orientation, glm::radians( 0.5f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
		renderWorld->UpdateEntity( entityHandle, entityParams );

		renderWorld->RenderFrame( rv );
		SDL_GL_SwapWindow( window );
	}

	// =====================================
	// STEP 7
	// Shut everything down
	renderWorld->Shutdown();
	SDL_Quit();
	return 0;
}
