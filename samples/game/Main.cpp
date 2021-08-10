#include <iostream>
#include "SDL.h"
#include "IEngine.hpp"

/*
	This shows an example of a mini "game" using FoxGLBox and SDL2.
*/

int main( int argc, char** argv )
{
	IEngine* engine = IEngine::AllocateInstance();
	engine->Init( "FoxGLBox Game Env", 1280, 720 );

	while ( engine->RunFrame() )
	{

	}

	engine->Shutdown();
	delete engine;
	
	return 0;
}
