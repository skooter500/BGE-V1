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

		static glm::vec3 BtToGLVector(const btVector3 & v);
		static glm::quat BtToGLQuat(const btQuaternion & q);
		
		static btVector3 GLToBtVector(const glm::vec3 & v);
		static btQuaternion GLToBtQuat(const glm::quat & q);

		void SetPhysicsStuff(btCollisionShape * shape, btRigidBody * PhysicsComponent, btMotionState * motionState);
		btCollisionShape * shape;
		btRigidBody * rigidBody;
		btMotionState * motionState;
	};
}
