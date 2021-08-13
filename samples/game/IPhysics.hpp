#pragma once

class PhysicsObject;

class IPhysics
{
public:
	virtual PhysicsObject* CreatePhysicsObject( const fglVector& position, const fglVector& rotation ) = 0;

	virtual void Init() = 0;
	virtual void Update( const float& deltaTime ) = 0;
};

extern IPhysics& GetPhysics();
