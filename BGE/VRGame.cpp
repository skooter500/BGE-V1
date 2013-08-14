#include "VRGame.h"
#include "Person.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include <sstream>
#include <string>

using namespace BGE;
using namespace std;

VRGame::VRGame(void)
{
	physicsFactory = NULL;
	dynamicsWorld = NULL;
	broadphase = NULL;
	dispatcher = NULL;
	solver = NULL;
	elapsed = 10000.0f;
	fireRate = 5.0f;
	leftHandPickedUp= NULL;
	rightHandPickedUp= NULL;
	id = "Physics Camera";
}

VRGame::~VRGame(void)
{
}

void CheckFMODResult( FMOD_RESULT res )
{
	if (res != FMOD_OK)
	{
		throw BGE::Exception(FMOD_ErrorString(res));
	}
}

bool VRGame::Initialise() 
{
	// Set up the collision configuration and dispatcher
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The world.
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	broadphase = new btAxisSweep3(worldMin,worldMax);
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-9,0));

	// FMOD
	
	CheckFMODResult(FMOD::System_Create(& fmodSystem));
	CheckFMODResult(fmodSystem->init(100, FMOD_INIT_NORMAL, 0));

	// Load some sounds
	for (int i = 0 ; i < 7 ; i ++)
	{

	}

	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);

	physicsFactory->CreateGroundPhysics();
	physicsFactory->CreateCameraPhysics();
	person = make_shared<Person>();
	AddChild(person);

	physicsFactory->CreateWall(glm::vec3(-20, 0, 20), 5, 5);

	fullscreen = true;
	renderToRift = true;
	person->headCamera = true;

	if (!Game::Initialise()) {
		return false;
	}

	return true;
}

void VRGame::FireProjectile(glm::vec3 pos, glm::vec3 look)
{
	glm::quat q(RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat());
	glm::normalize(q);
	shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateSphere(1, pos, q);

	float force = 1000.0f;
	physicsComponent->rigidBody->applyCentralForce(PhysicsController::GLToBtVector(look) * force);
}

// Note that pickedUp is passed by reference and so can be changed!!
void VRGame::GravityGun(SDL_Joystick * joy, int axis, PhysicsController * & pickedUp, Hand hand)
{
	int lhp = SDL_JoystickGetAxis(joy, axis);
	if (lhp > 0)
	{
		float dist = 1000.0f;
		if (pickedUp == NULL)
		{		
			btVector3 rayFrom = PhysicsController::GLToBtVector(hand.pos); // Has to be some distance in front of the camera otherwise it will collide with the camera all the time
			btVector3 rayTo = PhysicsController::GLToBtVector(hand.pos + (hand.look * dist));

			btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
			dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

			if (rayCallback.hasHit())
			{
				pickedUp = reinterpret_cast<PhysicsController*>(rayCallback.m_collisionObject->getUserPointer());
				if (pickedUp->parent == ground.get())
				{
					pickedUp = NULL;
				}
			}
		}
		if (pickedUp != NULL)
		{
			// Press Y to first the thing away
			int yb = SDL_JoystickGetButton(joy, 13);
			if (yb)
			{
				float force = 1000.0f;
				pickedUp->rigidBody->applyCentralForce(PhysicsController::GLToBtVector(hand.look) * force);
			}
			float powerfactor = 4.0f; // Higher values causes the targets moving faster to the holding point.
			float maxVel = 3.0f;      // Lower values prevent objects flying through walls.
			float holdDist = 5.0f;
			// Calculate the hold point in front of the camera
			glm::vec3 holdPos = hand.pos + (hand.look * holdDist);

			glm::vec3 v = holdPos - pickedUp->position; // direction to move the Target
			v *= powerfactor; // powerfactor of the GravityGun

			if (v.length() > maxVel)
			{
				// if the correction-velocity is bigger than maximum
				v = glm::normalize(v);
				v *= maxVel; // just set correction-velocity to the maximum
			}
			pickedUp->rigidBody->setLinearVelocity(PhysicsController::GLToBtVector(v));    
			pickedUp->rigidBody->activate();	

		}
	}
	else
	{
		pickedUp = NULL;
	}
}



void VRGame::Update(float timeDelta)
{
	dynamicsWorld->stepSimulation(timeDelta,100);

	// Override the one in the base class, we do not want to update our world transform from the physics object
	// WHich is what the one in the base class does...

	const Uint8 * keyState = Game::Instance()->GetKeyState();

	float moveSpeed = speed;
	float timeToPass = 1.0f / fireRate;

	elapsed += timeDelta;

	SDL_Joystick * joy;
	if (SDL_NumJoysticks() > 0) 
	{
		// Open joystick
		joy = SDL_JoystickOpen(0);
		if (joy) 
		{
			// Check left button
			int lb = SDL_JoystickGetButton(joy, 8);
			if (lb && (elapsed > timeToPass))
			{
				float dist = 3.0f;
				glm::vec3 pos = person->hands[0].pos + (person->hands[0].look * dist);
				FireProjectile(pos, person->hands[0].look);
				elapsed = 0.0f;
			}

			// Check right button
			int rb = SDL_JoystickGetButton(joy, 9);
			if (rb && (elapsed > timeToPass))
			{
				float dist = 3.0f;
				glm::vec3 pos = person->hands[1].pos + (person->hands[1].look * dist);
				FireProjectile(pos, person->hands[1].look);
				elapsed = 0.0f;
			}

			// Check the A key and spawn a car
			int ab = SDL_JoystickGetButton(joy, 10);
			if (ab && (elapsed > timeToPass))
			{
				glm::vec3 point;
				bool hit = ground->rayIntersectsWorldPlane(camera->position, camera->look, point);
				if (hit)
				{
					point.y = 5;
					physicsFactory->CreateVehicle(point);
				}
				elapsed = 0.0f;
			}

			// Check the B key and spawn a ball
			int bb = SDL_JoystickGetButton(joy, 11);
			if (bb && (elapsed > timeToPass))
			{
				glm::vec3 point;
				bool hit = ground->rayIntersectsWorldPlane(camera->position, camera->look, point);
				if (hit)
				{
					point.y = 5;
					physicsFactory->CreateSphere(3, point, glm::quat());
				}
				elapsed = 0.0f;
			}
			GravityGun(joy, 4, leftHandPickedUp, person->hands[0]);
			GravityGun(joy, 5, rightHandPickedUp, person->hands[1]);
		}
		

		if (SDL_JoystickGetAttached(joy)) {
			SDL_JoystickClose(joy);
		}
	}

		/*if ((keyState[SDL_SCANCODE_SPACE]) && (elapsed > timeToPass))
		{
		glm::vec3 pos = parent->position + (parent->look * 5.0f);
		glm::quat q(RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat());
		glm::normalize(q);
		shared_ptr<PhysicsController> physicsComponent = game->physicsFactory->CreateBox(1,1,1, pos, q);

		float force = 5000.0f;
		physicsComponent->rigidBody->applyCentralForce(GLToBtVector(parent->look) * force);
		elapsed = 0.0f;
		}
		else
		{
		elapsed += timeDelta;
		}
		string what = "Nothing";*/


	// Handle the gravity gun
	//if (SDL_GetMouseState(NULL, NULL) && SDL_BUTTON(3))
	//{
	//	float dist = 10.0f;
	//	if (pickedUp == NULL)
	//	{		
	//		btVector3 rayFrom = PhysicsCamera::GLToBtVector(parent->position + (parent->look * dist)); // Has to be some distance in front of the camera otherwise it will collide with the camera all the time
	//		btVector3 rayTo = GLToBtVector(parent->position + (parent->look * dist * dist));

	//		btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
	//		game->dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

	//		if (rayCallback.hasHit())
	//		{
	//			pickedUp = reinterpret_cast<PhysicsController*>(rayCallback.m_collisionObject->getUserPointer());
	//			if (pickedUp->parent == game->GetGround().get())
	//			{
	//				pickedUp = NULL;
	//			}
	//		}
	//	}
	//	if (pickedUp != NULL)
	//	{
	//		float powerfactor = 4.0f; // Higher values causes the targets moving faster to the holding point.
	//		float maxVel = 3.0f;      // Lower values prevent objects flying through walls.

	//		// Calculate the hold point in front of the camera
	//		glm::vec3 holdPos = parent->position + (parent->look * dist);

	//		glm::vec3 v = holdPos - pickedUp->position; // direction to move the Target
	//		v *= powerfactor; // powerfactor of the GravityGun

	//		if (v.length() > maxVel)
	//		{
	//			// if the correction-velocity is bigger than maximum
	//			v = glm::normalize(v);
	//			v *= maxVel; // just set correction-velocity to the maximum
	//		}
	//		pickedUp->rigidBody->setLinearVelocity(GLToBtVector(v));    
	//		pickedUp->rigidBody->activate();		
	//		what = pickedUp->id;	
	//	}
	//}
	//else
	//{    
	//	pickedUp = NULL;
	//}
	//stringstream ss;
	//ss << "Picked up: " << what;
	//game->PrintText(ss.str());

	Game::Update(timeDelta);
}

void VRGame::Cleanup()
{
	Game::Cleanup();
}
