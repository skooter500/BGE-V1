#include "VRGame2.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include <sstream>
#include <string>
#include "Content.h"
#include "Utils.h"
#include <sapi.h>
#include <sphelper.h>

using namespace BGE;
using namespace std;

bool collisionCallback2(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
{
	VRGame2 * game = (VRGame2 *) Game::Instance();
	PhysicsController * object0 = reinterpret_cast<PhysicsController*> (colObj0Wrap->getCollisionObject()->getUserPointer());
	PhysicsController * object1 = reinterpret_cast<PhysicsController*> (colObj1Wrap->getCollisionObject()->getUserPointer());

	// Swap if the ground is the first object
	if (object0->parent == game->ground.get())
	{
		PhysicsController * temp = object0;
		object0 = object1;
		object1 = object0;
	}

	// Is it just two pieces of the person? Then dont play a sound
	if ((object0->tag.find("Person") == 0) && (object1->tag.find("Person") == 0))
	{
		return false;
	}

	// Also if its a piece of the person touching the ground
	if ((object0->tag.find("Person") == 0) && (object1->parent == game->ground.get()))
	{
		return false;
	}

	
	//game->soundSystem->PlayHitSoundIfReady(object0, 5000);

	
	return false;
}


VRGame2::VRGame2(void)
{
	physicsFactory = nullptr;
	dynamicsWorld = nullptr;
	broadphase = nullptr;
	dispatcher = nullptr;
	solver = nullptr;
	person = nullptr;
	
	fireRate = 5.0f;
	width = 1280;
	height = 800;
	leftHandPickedUp= nullptr;
	rightHandPickedUp= nullptr;

	fullscreen = false;
	riftEnabled = false;

	tag = "VR Game";
}

VRGame2::~VRGame2(void)
{
}

void VRGame2::ResetScene()
{
	list<shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		shared_ptr<GameComponent> component = * it;
		if ((component->tag == "Box") || (component->tag == "Model") || (component->tag == "Cylinder") || (component->tag == "Sphere") )
		{
			shared_ptr<PhysicsController> physics = dynamic_pointer_cast<PhysicsController> (component->FindComponentByTag("PhysicsController"));
			if (physics != nullptr)
			{
				dynamicsWorld->removeRigidBody(physics->rigidBody);
			}
			component->alive = false;
		}
		it ++;		
	}
	
	physicsFactory->CreateWall(glm::vec3(-20, 0, 20), 5, 5);

	glm::quat q = glm::angleAxis(glm::degrees(glm::half_pi<float>()), glm::vec3(1, 0, 0));

	physicsFactory->CreateCylinder(10, 2, glm::vec3(-30, 5, 5), q);
}

bool VRGame2::Initialise() 
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
	dynamicsWorld->setGravity(btVector3(0,0,0));

	camera->transform->position = glm::vec3(-1,20,46);
	camera->transform->look = glm::vec3(0, 0, 1);

	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);

	physicsFactory->CreateCameraPhysics();
	physicsFactory->CreateGroundPhysics();

	gContactAddedCallback = collisionCallback2;

	person = make_shared<Person2>();
	Attach(person);
	person->headCamera = true;

	ResetScene();

	if (!Game::Initialise()) {
		return false;
	}

	return true;
}



// Note that pickedUp is passed by reference and so can be changed!!
void VRGame2::GravityGun(PhysicsController * & pickedUp, KinectHand * hand)
{
	if (hand->state == HandState::HandState_Closed)
	{
		float dist = 1000.0f;
		if (pickedUp == nullptr)
		{		
			btVector3 rayFrom = GLToBtVector(hand->pos); // Has to be some distance in front of the camera otherwise it will collide with the camera all the time
			btVector3 rayTo = GLToBtVector(hand->pos + (hand->look * dist));

			btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
			dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

			if (rayCallback.hasHit())
			{
				pickedUp = reinterpret_cast<PhysicsController*>(rayCallback.m_collisionObject->getUserPointer());
				if (pickedUp->parent == ground.get())
				{
					pickedUp = nullptr;
				}
			}
		}
		if (pickedUp != nullptr)
		{			
			float powerfactor = 4.0f; // Higher values causes the targets moving faster to the holding point.
			float maxVel = 3.0f;      // Lower values prevent objects flying through walls.
			float holdDist = 5.0f;
			// Calculate the hold point in front of the camera
			glm::vec3 holdPos = hand->pos + (hand->look * holdDist);

			glm::vec3 v = holdPos - pickedUp->transform->position; // direction to move the Target
			v *= powerfactor; // powerfactor of the GravityGun

			if (v.length() > maxVel)
			{
				// if the correction-velocity is bigger than maximum
				v = glm::normalize(v);
				v *= maxVel; // just set correction-velocity to the maximum
			}
			pickedUp->rigidBody->setLinearVelocity(GLToBtVector(v));    
			pickedUp->rigidBody->activate();	
			Game::PrintText("Picked up: " + pickedUp->tag);
		}
	}
	else
	{
		pickedUp = nullptr;
	}
}


void VRGame2::Update(float timeDelta)
{

	string leftHandWhat = "Nothing";
	string rightHandWhat = "Nothing";

	dynamicsWorld->stepSimulation(timeDelta,100);

	const Uint8 * keyState = Game::Instance()->GetKeyState();

	float moveSpeed = speed;
	float timeToPass = 1.0f / fireRate;

	elapsed += timeDelta;

	PrintText("Press R to reset scene");
	static bool lastPressed = false;
	if (keyState[SDL_SCANCODE_R])
	{
		if (! lastPressed)
		{
			ResetScene();
		}
		lastPressed = true;
	}
	else
	{
		lastPressed = false;
	}	

	if (person)
	{
		GravityGun(leftHandPickedUp, & person->hands[0]);
		GravityGun(rightHandPickedUp, & person->hands[1]);
	}

	/*
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
				bool hit = ground->rayIntersectsWorldPlane(camera->transform->position, camera->transform->look, point);
				if (hit)
				{
					point.y = 5;
					physicsFactory->CreateVehicle(point);
					soundSystem->PlaySound("spawn", point);
				}
				elapsed = 0.0f;
			}

			// Check the B key and spawn a ball
			int bb = SDL_JoystickGetButton(joy, 11);
			if (bb && (elapsed > timeToPass))
			{
				glm::vec3 point;
				bool hit = ground->rayIntersectsWorldPlane(camera->transform->position, camera->transform->look, point);
				if (hit)
				{
					point.y = 5;
					physicsFactory->CreateSphere(3, point, glm::quat());
					soundSystem->PlaySound("spawn", point);
				}
				elapsed = 0.0f;
			}
			// Check the X key to spawn a random object
			int xb = SDL_JoystickGetButton(joy, 12);
			if (xb && (elapsed > timeToPass))
			{
				glm::vec3 point;
				bool hit = ground->rayIntersectsWorldPlane(camera->transform->position, camera->transform->look, point);
				if (hit)
				{
					point.y = 5;
					physicsFactory->CreateRandomObject(point, glm::quat());
					soundSystem->PlaySound("spawn", point);
				}
				elapsed = 0.0f;
			}

			if (person)
			{
				GravityGun(joy, 4, leftHandPickedUp, person->hands[0]);
				GravityGun(joy, 5, rightHandPickedUp, person->hands[1]);
			}
		}
		
		if (SDL_JoystickGetAttached(joy)) {
			SDL_JoystickClose(joy);
		}
	}
	*/
	Game::Update(timeDelta);
}

void VRGame2::Cleanup()
{
	Game::Cleanup();
}
