#pragma once
#include "Game.h"
#include "PhysicsFactory.h"
#include <btBulletDynamicsCommon.h>

namespace BGE
{
	class SceneGraphGame :
		public Game
	{
	private:
		btBroadphaseInterface* broadphase;

		// Set up the collision configuration and dispatcher
		btDefaultCollisionConfiguration * collisionConfiguration;
		btCollisionDispatcher * dispatcher;

		// The actual physics solver
		btSequentialImpulseConstraintSolver * solver;

	public:
		SceneGraphGame(void);
		~SceneGraphGame(void);
		bool Initialise();
		void Update(float timeDelta);
		void Cleanup();
		
		std::shared_ptr<PhysicsFactory> physicsFactory;
		btDiscreteDynamicsWorld * dynamicsWorld;
	};
}

