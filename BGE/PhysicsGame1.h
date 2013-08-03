#pragma once
#include "Game.h"
#include "PhysicsComponent.h"
#include <btBulletDynamicsCommon.h>

namespace BGE
{
	class PhysicsGame1 :
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
		PhysicsGame1(void);
		~PhysicsGame1(void);
		bool Initialise();
		void Update(float timeDelta);
		void Cleanup();
		void CreateWall();
		PhysicsComponent * CreateBox(float width, float height, float depth, glm::vec3 pos, glm::quat quat);
		PhysicsComponent * CreateSphere(float radius, glm::vec3 pos, glm::quat quat);
		PhysicsComponent * CreateCylinder(float radius, float height, glm::vec3 pos, glm::quat quat);
		PhysicsComponent * CreateVehicle(glm::vec3 pos);
		PhysicsComponent * CreateCameraPhysics();
		PhysicsComponent * CreateGroundPhysics();

		// The world.
		btDiscreteDynamicsWorld * dynamicsWorld;
	};
}
