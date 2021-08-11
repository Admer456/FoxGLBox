#pragma once

#include <string>

namespace Entities
{
	constexpr int MaxGameEntities = 1024;

	class GameEntity
	{
	public:
		GameEntity()
		{

		}

		~GameEntity()
		{

		}

		template<typename EntityClass>
		static EntityClass* Instance( fglVector position, fglVector rotation, RenderModelHandle modelHandle )
		{
			EntityClass* entity = new EntityClass();
			entity->position = position;
			entity->rotation = rotation;
			entity->renderParams.model = modelHandle;
			return entity;
		}

		virtual void	Spawn();
		virtual void	Update( const float& deltaTime );
		virtual void	Present();

		void			CalculateRenderParams();

		fglVector		position{ fglVector::Zero };
		fglVector		rotation{ fglVector::Zero };

		RenderEntityHandle renderHandle{ RenderHandleInvalid };
		RenderEntityParams renderParams{};

		struct EntityFlags
		{
			bool canThink : 1;
			bool visible : 1;
		} entityFlags;
	};

	class Prop : public GameEntity
	{
	public:
		virtual void	Spawn() override;
		void			Present() override;
	};

	class PropRotating : public Prop
	{
	public:
		void			Spawn() override;
		void			Update( const float& deltaTime ) override;
	};

	class Player final : public GameEntity
	{
	public:
		using InputFlags = uint8_t;
		enum : uint8_t
		{
			InputForward	= 1 << 0,
			InputBackward	= 1 << 1,
			InputLeft		= 1 << 2,
			InputRight		= 1 << 3,
			InputJump		= 1 << 4,
			InputCrouch		= 1 << 5,
			InputReload		= 1 << 6,
			InputShoot		= 1 << 7
		};

	public:
		void			Spawn() override;
		void			Update( const float& deltaTime ) override;
		void			Present() override;

	private:
		void			UpdateInput();
		void			UpdateMovement( const float& deltaTime );
		void			UpdateCamera();
		void			CalculateRenderView();

		RenderView		camera;
		fglVector		cameraPosition{ fglVector::Zero };
		fglVector		cameraLookDirection{ fglVector::Forward };
		float			cameraRoll{ 0.0f };

		float			mouseX{ 0.0f }, mouseY{ 0.0f };
		InputFlags		inputFlags{ 0U };
	};
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
