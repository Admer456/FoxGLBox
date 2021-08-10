#include "IEngine.hpp"
#include "IRenderWorld.hpp"
#include "Vector.hpp"
#include "GameEntity.hpp"
#include "SDL.h"
#include "Engine.hpp"

#include "glm/gtc/matrix_transform.hpp"

using namespace Entities;

//============================
// GameEntity
//============================

void GameEntity::Spawn()
{

}

void GameEntity::Update( const float& deltaTime )
{

}

void GameEntity::Present()
{

}

void GameEntity::CalculateRenderParams()
{
	renderParams.renderMask = 0;

	renderParams.position.x = position.x;
	renderParams.position.y = position.y;
	renderParams.position.z = position.z;

	// Orientation matrix
	glm::mat4 orient = glm::identity<glm::mat4>();
	// Pitch - rotate about right axis
	orient = glm::rotate( orient, glm::radians( rotation.x ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
	// Yaw - rotate about up axis
	orient = glm::rotate( orient, glm::radians( rotation.y ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
	// Roll - rotate about forward axis
	orient = glm::rotate( orient, glm::radians( rotation.z ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
	renderParams.orientation = orient;
}

//============================
// Prop
//============================

void Prop::Spawn()
{
	entityFlags.canThink = false;
	entityFlags.visible = true;

	CalculateRenderParams();
	renderHandle = gEngine->GetRenderWorld()->CreateEntity( renderParams );

	if ( renderHandle == RenderHandleInvalid )
	{
		gEngine->Print( "Prop::Spawn: render handle invalid\n" );
	}
}

void Prop::Present()
{
}

//============================
// PropRotating
//============================

void PropRotating::Spawn()
{
	Prop::Spawn();
	entityFlags.canThink = true;
}

void PropRotating::Update( const float& deltaTime )
{
	Prop::Update( deltaTime );

	CalculateRenderParams();
	gEngine->GetRenderWorld()->UpdateEntity( renderHandle, renderParams );

	rotation.y += deltaTime * 15.0f;
}

//============================
// Player
//============================

void Player::Spawn()
{
	entityFlags.canThink = true;
	entityFlags.visible = true;

	gEngine->GetViewport( camera.viewportWidth, camera.viewportHeight );
	camera.cameraFov = 90.0f;

	CalculateRenderView();
}

void Player::Update( const float& deltaTime )
{
	UpdateInput();
	UpdateMovement( deltaTime );
	UpdateCamera();

	if ( inputFlags & InputReload )
	{
		gEngine->GetRenderWorld()->ReloadShaders();
	}
}

void Player::Present()
{
	gEngine->SubmitRenderView( camera, true );
}

void Player::UpdateInput()
{
	struct InputBinding
	{
		int sdlScanCode;
		int inputFlag;
	};

	constexpr InputBinding bindings[] =
	{
		{ SDL_SCANCODE_W,		InputForward },
		{ SDL_SCANCODE_S,		InputBackward },
		{ SDL_SCANCODE_A,		InputLeft },
		{ SDL_SCANCODE_D,		InputRight },
		{ SDL_SCANCODE_R,		InputReload },
		{ SDL_SCANCODE_SPACE,	InputJump },
		{ SDL_SCANCODE_LCTRL,	InputCrouch },
	};

	inputFlags = 0;

	// Key presses
	auto keyboard = SDL_GetKeyboardState( nullptr );
	for ( const auto& binding : bindings )
	{
		if ( keyboard[binding.sdlScanCode] )
		{
			inputFlags |= binding.inputFlag;
		}
	}

	// Mouse movement & LMB pressing
	int mx, my;
	auto mouseState = SDL_GetRelativeMouseState( &mx, &my );
	mouseX = mx * 0.1f;
	mouseY = my * 0.1f;

	if ( mouseState & SDL_BUTTON( SDL_BUTTON_LEFT ) )
	{
		inputFlags |= InputShoot;
	}
}

void Player::UpdateMovement( const float& deltaTime )
{
	fglVector localRight{
		cos( glm::radians( -rotation.y - 90.0f ) ) * cos( glm::radians( rotation.z ) ),
		sin( glm::radians( -rotation.y - 90.0f ) ) * cos( glm::radians( rotation.z ) ),
		sin( glm::radians( rotation.z ) )
	};

	const float PlayerSpeed = 5.0f * deltaTime;

	if ( inputFlags & InputForward )
	{
		position += cameraLookDirection * PlayerSpeed;
	}
	if ( inputFlags & InputBackward )
	{
		position -= cameraLookDirection * PlayerSpeed;
	}
	if ( inputFlags & InputRight )
	{
		position += localRight * PlayerSpeed;
	}
	if ( inputFlags & InputLeft )
	{
		position -= localRight * PlayerSpeed;
	}
}

void Player::UpdateCamera()
{
	rotation.y += mouseX;
	rotation.x += mouseY;

	if ( rotation.x > 89.9f )
	{
		rotation.x = 89.9f;
	}
	if ( rotation.x < -89.9f )
	{
		rotation.x = 89.9f;
	}

	cameraPosition = position + fglVector( 0.0f, 0.0f, 1.7f );
	cameraLookDirection.x = cos( glm::radians( -rotation.y ) ) * cos( glm::radians( rotation.x ) );
	cameraLookDirection.y = sin( glm::radians( -rotation.y ) ) * cos( glm::radians( rotation.x ) );
	cameraLookDirection.z = sin( glm::radians( rotation.x ) );
	cameraRoll = rotation.z;

	CalculateRenderView();
}

void Player::CalculateRenderView()
{
	camera.cameraPosition.x = cameraPosition.x;
	camera.cameraPosition.y = cameraPosition.y;
	camera.cameraPosition.z = cameraPosition.z;

	glm::mat4 orient = glm::identity<glm::mat4>();
	glm::vec3 camDir{ cameraLookDirection.x, cameraLookDirection.y, cameraLookDirection.z };

	const glm::vec3 camCentre = glm::vec3( 0.0f, 0.0f, 0.0f );
	const glm::vec3 camUp = glm::vec3( 0.0f, 0.0f, 1.0f );

	// Look in the direction the camera's facing
	orient = glm::lookAt( camDir * 0.1f, camCentre, camUp );
	// Roll the camera
	orient = glm::rotate( orient, glm::radians( cameraRoll ), camDir );

	camera.cameraOrientation = orient;
}
