#pragma once
#include "Game.h"
#include "PhysicsController.h"
#include "PhysicsFactory.h"
#include "Person.h"
#include <btBulletDynamicsCommon.h>
#include <fmod.hpp>
#include "KinectFlyingController.h"

namespace BGE
{

	class VRGame :
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
		VRGame(void);
		~VRGame(void);
		bool Initialise();
		void Update(float timeDelta);
		void Cleanup();

		void FireProjectile(glm::vec3 pos, glm::vec3 look);
		void GravityGun(SDL_Joystick * joy, int axis, PhysicsController * & leftHandPickedUp, Hand hand);
		void ResetScene();
		
		// The world.
		std::shared_ptr<PhysicsFactory> physicsFactory;
		btDiscreteDynamicsWorld * dynamicsWorld;
		shared_ptr<KinectFlyingController> kfc;
		shared_ptr<GameComponent> flyThing;
		
		// Now stuff we need to track
		shared_ptr<Person> person;
		PhysicsController * leftHandPickedUp;
		PhysicsController * rightHandPickedUp;
		shared_ptr<GameComponent> camFollower;
		bool camFollowing;
		float fireRate;
		bool high;

	};
}
