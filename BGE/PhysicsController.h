#pragma once
#include "GameComponent.h"
#include <btBulletDynamicsCommon.h>

namespace BGE
{
	class PhysicsController :
		public GameComponent
	{
	public:
		PhysicsController(btCollisionShape * shape, btRigidBody * PhysicsComponent, btMotionState * motionState);
		PhysicsController();
		~PhysicsController(void);
		void Update(float timeDelta);
		void Cleanup();

		void SetPhysicsStuff(btCollisionShape * shape, btRigidBody * PhysicsComponent, btMotionState * motionState);
		btCollisionShape * shape;
		btRigidBody * rigidBody;
		btMotionState * motionState;
	};
}
