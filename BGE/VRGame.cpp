#include "VRGame.h"
#include "Person.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include <sstream>
#include <string>
#include "Content.h"
#include "Utils.h"
using namespace BGE;
using namespace std;

bool collisionCallback(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
{
	VRGame * game = (VRGame *) Game::Instance();
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

	
	game->soundSystem->PlayHitSoundIfReady(object0, 5000);

	
	return false;
}


VRGame::VRGame(void)
{
	physicsFactory = NULL;
	dynamicsWorld = NULL;
	broadphase = NULL;
	dispatcher = NULL;
	solver = NULL;
	person = NULL;
	
	fireRate = 5.0f;
	width = 1280;
	height = 800;
	leftHandPickedUp= NULL;
	rightHandPickedUp= NULL;

	fullscreen = true;
	riftEnabled = true;

	tag = "VR Game";
}

VRGame::~VRGame(void)
{
}

void VRGame::ResetScene()
{

	list<shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		shared_ptr<GameComponent> component = * it;
		if ((component->tag == "Box") || (component->tag == "Model") || (component->tag == "Cylinder") || (component->tag == "Sphere"))
		{
			shared_ptr<PhysicsController> physics = dynamic_pointer_cast<PhysicsController> (component->GetController());
			dynamicsWorld->removeRigidBody(physics->rigidBody);
			it = children.erase(it);
		}
		else
		{
			it ++;
		}
	}
	
	physicsFactory->CreateWall(glm::vec3(-20, 0, 20), 5, 5);
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

	camera->position = glm::vec3(-1,20,46);
	camera->look = glm::vec3(0, 0, 1);

	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);

	physicsFactory->CreateCameraPhysics();
	physicsFactory->CreateGroundPhysics();

	gContactAddedCallback = collisionCallback;

	person = make_shared<Person>();
	Attach(person);
	person->headCamera = true;

	ResetScene();

	if (!Game::Initialise()) {
		return false;
	}

	camera->GetController()->position = camera->position;
	camera->GetController()->look = camera->look;

	return true;
}

void VRGame::FireProjectile(glm::vec3 pos, glm::vec3 look)
{
	glm::quat q(RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat());
	glm::normalize(q);
	shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateSphere(1, pos, q);
	soundSystem->PlaySound("Fire", pos);
	//soundSystem->Vibrate(200, 1.0f);
	float force = 3000.0f;
	physicsComponent->rigidBody->applyCentralForce(GLToBtVector(look) * force);
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
			btVector3 rayFrom = GLToBtVector(hand.pos); // Has to be some distance in front of the camera otherwise it will collide with the camera all the time
			btVector3 rayTo = GLToBtVector(hand.pos + (hand.look * dist));

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
			// Press Y to throw the thing away
			int yb = SDL_JoystickGetButton(joy, 13);
			if (yb)
			{
				float force = 300.0f;
				pickedUp->rigidBody->applyCentralForce(GLToBtVector(hand.look) * force);
				pickedUp = NULL;
				return;
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
			pickedUp->rigidBody->setLinearVelocity(GLToBtVector(v));    
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
					soundSystem->PlaySound("spawn", point);
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
					soundSystem->PlaySound("spawn", point);
				}
				elapsed = 0.0f;
			}
			// Check the X key to spawn a random object
			int xb = SDL_JoystickGetButton(joy, 12);
			if (xb && (elapsed > timeToPass))
			{
				glm::vec3 point;
				bool hit = ground->rayIntersectsWorldPlane(camera->position, camera->look, point);
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

	Game::Update(timeDelta);
}

void VRGame::Cleanup()
{
	Game::Cleanup();
}
