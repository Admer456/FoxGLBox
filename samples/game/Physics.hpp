#pragma once

#include "PxPhysicsAPI.h"

class PhysicsObject
{
public:
	void	Setup( const fglVector& position, const fglVector& rotation );
	void	GetTransform( fglVector& outPosition, fglVector& outRotation );

	bool	IsActive() const { return active; }
protected:
	bool	active{ false };

	physx::PxRigidDynamic* physRigidBody{ nullptr };
	physx::PxTransform physTransform;

	inline static physx::PxShape* physDefaultShape{ nullptr };
};

class Physics final : public IPhysics
{
public:
	PhysicsObject*					CreatePhysicsObject( const fglVector& position, const fglVector& rotation ) override;

	void							Init() override;
	void							Update( const float& deltaTime ) override;

public:
	physx::PxPhysics*				GetPhysics() { return pxPhysics; }
	physx::PxScene*					GetScene() { return physScene; }
	physx::PxMaterial*				GetDefaultMaterial() { return physDefaultMaterial; }

private:
	PhysicsObject					objects[512];

	physx::PxDefaultAllocator		physAllocator;
	physx::PxDefaultErrorCallback	physErrorCallback;

	physx::PxFoundation*			physFoundation{ nullptr };
	physx::PxPhysics*				pxPhysics{ nullptr };
	physx::PxDefaultCpuDispatcher*	physDispatcher{ nullptr };

	physx::PxScene*					physScene{ nullptr };
	physx::PxMaterial*				physDefaultMaterial{ nullptr };
};

extern Physics gPhysics;
