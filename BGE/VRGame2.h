#pragma once
#include "Game.h"
#include "PhysicsController.h"
#include "PhysicsFactory.h"
#include "Person2.h"
#include <btBulletDynamicsCommon.h>
#include <fmod.hpp>
#include "LeapHands.h"

namespace BGE
{

	class VRGame2 :
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
		VRGame2(void);
		~VRGame2(void);
		bool Initialise();
		void Update(float timeDelta);
		void Cleanup();

		void FireProjectile(glm::vec3 pos, glm::vec3 look);
		//void GravityGun(SDL_Joystick * joy, int axis, PhysicsController * & leftHandPickedUp, Hand hand);
		void ResetScene();
		
		// The world.
		void VRGame2::GravityGun(PhysicsController * & pickedUp, KinectHand * hand);
		
		// Now stuff we need to track
		shared_ptr<Person2> person;
		PhysicsController * leftHandPickedUp;
		PhysicsController * rightHandPickedUp;

		shared_ptr<LeapHands> leapHands;

		float fireRate;
	};
}
