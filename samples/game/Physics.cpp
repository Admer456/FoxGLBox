#include "Vector.hpp"
#include "IPhysics.hpp"
#include "Physics.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

Physics gPhysics;

IPhysics& GetPhysics()
{
	return gPhysics;
}

// PhysX uses a right-handed coordinate system:
// Up is +Y			to our +Z
// Forward is -Z	to our +Y
// Right is +X		to our +X
fglVector PxVecToFglVector( const physx::PxVec3& vec )
{
	return fglVector{
		vec.x,
		-vec.z,
		vec.y
	};
}

// PhysX uses a right-handed coordinate system:
// Our up is +Z			to their +Y
// Our forward is +Y	to their -Z
// Our right is +X		to their +X
physx::PxVec3 FglVectorToPxVec( const fglVector& vec )
{
	return physx::PxVec3{
		vec.x,
		vec.z,
		-vec.y
	};
}

// From: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
fglVector QuatToAngles( const physx::PxQuat& q )
{
	// In our coordinate system, roll is Y axis rotation, while pitch is X axis rotation
	// So we'll just swap the two from the original Wiki article
	fglVector angles;

	// Pitch (X axis rotation)
	float sinr_cosp = 2.0f * (q.w*q.x + q.y*q.z);
	float cosr_cosp = 1.0f - 2.0f * (q.x*q.x + q.y*q.y);
	angles.x = std::atan2( sinr_cosp, cosr_cosp );

	// Roll (Y axis rotation)
	float sinp = 2.0f * (q.w * q.y - q.z * q.x);
	if ( std::abs( sinp ) >= 1.0f )
		angles.z = std::copysign( M_PI / 2.0f, sinp ); // use 90 degrees if out of range
	else
		angles.z = std::asin( sinp );

	// Yaw (Z axis rotation)
	float siny_cosp = 2.0f * (q.w*q.z + q.x*q.y);
	float cosy_cosp = 1.0f - 2.0f * (q.y*q.y + q.z*q.z);
	angles.y = std::atan2( siny_cosp, cosy_cosp );
	
	return angles * (180.0f / M_PI);
}

void PhysicsObject::Setup( const fglVector& position, const fglVector& rotation )
{
	if ( nullptr == physDefaultShape )
	{
		physx::PxBoxGeometry box( 0.5f, 0.5f, 0.5f );
		physx::PxMaterial& material = *gPhysics.GetDefaultMaterial();

		physDefaultShape = gPhysics.GetPhysics()->createShape( box, material );
	}
	
	physTransform = physx::PxTransform( FglVectorToPxVec( position ) );
	physRigidBody = physx::PxCreateDynamic( *gPhysics.GetPhysics(), physTransform, *physDefaultShape, 10.0f );
	physRigidBody->setLinearDamping( 0.1f );

	gPhysics.GetScene()->addActor( *physRigidBody );

	active = true;
}

void PhysicsObject::GetTransform( fglVector& outPosition, fglVector& outRotation )
{
	physTransform = physRigidBody->getGlobalPose();

	outPosition = PxVecToFglVector( physTransform.p );
	outRotation = QuatToAngles( physTransform.q );
}

PhysicsObject* Physics::CreatePhysicsObject( const fglVector& position, const fglVector& rotation )
{
	for ( PhysicsObject& phys : objects )
	{
		if ( !phys.IsActive() )
		{
			phys.Setup( position, rotation );
			return &phys;
		}
	}

	return nullptr;
}

void Physics::Init()
{
	// Foundation
	physFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, physAllocator, physErrorCallback );
	
	/* // PhysX Visual Debugger
	physPvd = PxCreatePvd( *physFoundation );
	physx::PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
	physPvd->connect( *transport, PxPvdInstrumentationFlag::eALL );
	*/

	// Physics
	pxPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *physFoundation, physx::PxTolerancesScale(), true/*, physPvd*/ );
	// Gravity setup
	physx::PxSceneDesc sceneDesc( pxPhysics->getTolerancesScale() );
	sceneDesc.gravity = physx::PxVec3( 0.0f, -9.81f, 0.0f );
	// CPU dispatcher
	physDispatcher = physx::PxDefaultCpuDispatcherCreate( 2 );
	// Scene setup
	sceneDesc.cpuDispatcher = physDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	// Init the physics system
	physScene = pxPhysics->createScene( sceneDesc );

	/* // More PVD stuff
	physx::PxPvdSceneClient* pvdClient = physScene->getScenePvdClient();
	if(pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	*/

	// Default material pls
	physDefaultMaterial = pxPhysics->createMaterial( 0.5f, 0.5f, 0.6f );

	// Also create a floor plane so we don't fall to infinity and beyond
	physx::PxRigidStatic* groundPlane = physx::PxCreatePlane( *pxPhysics, physx::PxPlane( 0, 1, 0, -2.0f ), *physDefaultMaterial );
	physScene->addActor( *groundPlane );

	printf( "==== Physics system initialised\n" );
}

void Physics::Update( const float& deltaTime )
{
	physScene->simulate( deltaTime );
	physScene->fetchResults( true );
}
