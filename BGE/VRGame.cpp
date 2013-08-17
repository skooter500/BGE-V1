#include "VRGame.h"
#include "Person.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include <sstream>
#include <string>
#include "Content.h"

#define NUM_HIT_SOUNDS 7

using namespace BGE;
using namespace std;

FMOD_VECTOR GLToFMODVector(glm::vec3 v)
{
	FMOD_VECTOR fv;
	fv.x = v.x;
	fv.y = v.y;
	fv.z = v.z;
	return fv;
}

struct SoundEvent
{
	FMOD::Sound * sound;
	FMOD::Channel * channel;
	long last;
};

map<GameComponent *, SoundEvent> soundEvents;

bool collisionCallback(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
{
	long interval = 5000; // Allow X ms to elapse before allowing an object to play a sound
	long now = SDL_GetTicks();
	VRGame * game = (VRGame *) Game::Instance();
	PhysicsController * object = reinterpret_cast<PhysicsController*> (colObj0Wrap->getCollisionObject()->getUserPointer());
	
	if (object->parent == game->ground.get())
	{
		object = (PhysicsController *) reinterpret_cast<PhysicsController*> (colObj1Wrap->getCollisionObject()->getUserPointer());
	}

	// Has this object already played a sound?
	map<GameComponent *, SoundEvent>::iterator it = soundEvents.find(object);
	SoundEvent soundEvent;
	bool isPlaying = false;
	if (it != soundEvents.end())
	{
		if (now - it->second.last > interval)
		{
			// Is the sound already playing?
			soundEvent = it->second;
			soundEvent.channel->isPlaying(& isPlaying);
			if (isPlaying)
			{
				return false;
			}
		}
		else
		{
			// Its too soon to play this object's sound
			return false;
		}
	}
	else
	{
		int which = rand() % NUM_HIT_SOUNDS;
		stringstream ss;
		ss << "Hit" << which;
		soundEvent.sound = Content::LoadSound(ss.str(), game->fmodSystem);
	}
	
	game->fmodSystem->playSound(FMOD_CHANNEL_FREE, soundEvent.sound, false, & soundEvent.channel);
	if (soundEvent.channel != NULL)
	{
		if (isPlaying)
		{
			soundEvent.channel->set3DAttributes(& GLToFMODVector(object->position), & GLToFMODVector(glm::vec3(0)));
		}
	}
	soundEvent.last = now;
	soundEvents[object] = soundEvent;
	return false;
}


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

	fullscreen = false;
	riftEnabled = true;

	id = "VR Game";
}

VRGame::~VRGame(void)
{
}

void CheckFMODResult( FMOD_RESULT res )
{
	if (res != FMOD_OK)
	{
		const char * error = FMOD_ErrorString(res);
		throw BGE::Exception(error);
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

	camera->position = glm::vec3(-1,20,46);

	// FMOD
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS caps;
	CheckFMODResult(FMOD::System_Create(& fmodSystem));
	CheckFMODResult(fmodSystem->getDriverCaps(0, &caps, 0, &speakerMode));
	CheckFMODResult(fmodSystem->setSpeakerMode(speakerMode));

	CheckFMODResult(fmodSystem->init(1000, FMOD_INIT_NORMAL, 0));

	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);

	physicsFactory->CreateGroundPhysics();
	physicsFactory->CreateCameraPhysics();
	person = make_shared<Person>();
	AddChild(person);

	physicsFactory->CreateWall(glm::vec3(-20, 0, 20), 5, 5);

	//gContactAddedCallback = collisionCallback;

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
			// Press Y to throw the thing away
			int yb = SDL_JoystickGetButton(joy, 13);
			if (yb)
			{
				float force = 1000.0f;
				pickedUp->rigidBody->applyCentralForce(PhysicsController::GLToBtVector(hand.look) * force);
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
	string leftHandWhat = "Nothing";
	string rightHandWhat = "Nothing";

	dynamicsWorld->stepSimulation(timeDelta,100);

	fmodSystem->set3DListenerAttributes(0,
		& GLToFMODVector(camera->position)
		, & GLToFMODVector(glm::vec3(0))
		, & GLToFMODVector(camera->look)
		, & GLToFMODVector(camera->up)
		);
	fmodSystem->update();

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

		
	

	Game::Update(timeDelta);
}

void VRGame::Cleanup()
{
	Game::Cleanup();
}
