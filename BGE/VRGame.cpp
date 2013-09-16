#include "VRGame.h"
#include "Person.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include <sstream>
#include <string>
#include "Content.h"
#include "Utils.h"
#include "KinectFlyingController.h"
#include "Params.h"
using namespace BGE;
using namespace std;

float findDist = 50;

vector<shared_ptr<GameComponent>> removable;

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

	//game->soundSystem->PlayHitSoundIfReady(object0, 5000);

	
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
	high = false;
	fireRate = 5.0f;
	width = 800;
	height = 600;
	leftHandPickedUp= NULL;
	rightHandPickedUp= NULL;

	fullscreen = false;
	riftEnabled = false;
	camFollowing = false;

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
	
	//physicsFactory->CreateWall(glm::vec3(-20, 0, 20), 5, 5);
}

bool VRGame::Initialise() 
{
	Params::Load("default");

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

	camera->position = glm::vec3(-1,30,46);
	camera->look = glm::vec3(0, 0, 1);

	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);

	physicsFactory->CreateCameraPhysics();
	physicsFactory->CreateGroundPhysics();

	gContactAddedCallback = collisionCallback;

	/*person = make_shared<Person>();
	Attach(person);
	person->headCamera = true;
	*/
	
	flyThing = make_shared<GameComponent>();	
	shared_ptr<Model> flyModel = Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp));
	flyThing->drawMode = GameComponent::draw_modes::materials;
	kfc = make_shared<KinectFlyingController>(flyModel);
	flyThing->Attach(kfc);
	kfc->position = glm::vec3(0, 20, -10);
	//flyThing->Attach(flyModel);
	Attach(flyThing);

	
	shared_ptr<GameComponent> floor = make_shared<GameComponent>();
	floor->drawMode = GameComponent::draw_modes::textured;
	floor->Attach(Content::LoadModel("gccontent/floor_01"));
	floor->position = glm::vec3(0, 0, 0);
	Attach(floor);
	removable.push_back(floor);

	shared_ptr<GameComponent> ceiling = make_shared<GameComponent>();
	ceiling->drawMode = GameComponent::draw_modes::textured;
	ceiling->Attach(Content::LoadModel("gccontent/ceiling"));
	ceiling->position = glm::vec3(0, 0, 0);
	Attach(ceiling);
	removable.push_back(ceiling);

	shared_ptr<GameComponent> childtable = make_shared<GameComponent>();
	childtable->drawMode = GameComponent::draw_modes::textured;
	childtable->Attach(Content::LoadModel("gccontent/childtable"));
	childtable->position = glm::vec3(0, 0, 0);
	Attach(childtable);

	shared_ptr<GameComponent> wall1 = make_shared<GameComponent>();
	wall1->drawMode = GameComponent::draw_modes::textured;
	wall1->Attach(Content::LoadModel("gccontent/wall_01"));
	wall1->position = glm::vec3(0, 0, 0);
	Attach(wall1);
	removable.push_back(wall1);

	shared_ptr<GameComponent> wall2 = make_shared<GameComponent>();
	wall2->drawMode = GameComponent::draw_modes::textured;
	wall2->Attach(Content::LoadModel("gccontent/wall_02"));
	wall2->position = glm::vec3(0, 0, 0);
	Attach(wall2);
	removable.push_back(wall2);

	shared_ptr<GameComponent> wall3 = make_shared<GameComponent>();
	wall3->drawMode = GameComponent::draw_modes::textured;
	wall3->Attach(Content::LoadModel("gccontent/wall_03"));
	wall3->position = glm::vec3(0, 0, 0);
	Attach(wall3);
	removable.push_back(wall3);

	shared_ptr<GameComponent> wall4 = make_shared<GameComponent>();
	wall4->drawMode = GameComponent::draw_modes::textured;
	wall4->Attach(Content::LoadModel("gccontent/wall_04"));
	wall4->position = glm::vec3(0, 0, 0);
	Attach(wall4);
	removable.push_back(wall4);

	shared_ptr<GameComponent> table = make_shared<GameComponent>();
	table->drawMode = GameComponent::draw_modes::textured;
	table->Attach(Content::LoadModel("gccontent/table01"));
	table->position = glm::vec3(0, 0, 0);
	Attach(table);
	
	shared_ptr<GameComponent> book = make_shared<GameComponent>();
	book->drawMode = GameComponent::draw_modes::textured;
	book->Attach(Content::LoadModel("gccontent/books"));
	book->position = glm::vec3(0, 0, 0);
	Attach(book);


	shared_ptr<GameComponent> scott = make_shared<GameComponent>();
	scott->drawMode = GameComponent::draw_modes::textured;
	scott->scale = glm::vec3(0.2f, 0.2f, 0.2f);
	scott->Attach(Content::LoadModel("gccontent/Mushroom"));
	scott->tag = "Mushroom";
	scott->position = glm::vec3(0, 18, -70);
	Attach(scott);

	{
		shared_ptr<GameComponent> scott = make_shared<GameComponent>();
		scott->drawMode = GameComponent::draw_modes::textured;
		scott->Attach(Content::LoadModel("gccontent/Mushroom"));
		scott->tag = "Mushroom";
		scott->position = glm::vec3(-150, 0, -170);
		Attach(scott);
	}

	{
		shared_ptr<GameComponent> scott = make_shared<GameComponent>();
		scott->drawMode = GameComponent::draw_modes::textured;
		scott->Attach(Content::LoadModel("gccontent/Mushroom"));
		scott->tag = "Mushroom";
		scott->position = glm::vec3(150, 0, -170);
		Attach(scott);
	}

	{
		shared_ptr<GameComponent> scott = make_shared<GameComponent>();
		scott->drawMode = GameComponent::draw_modes::textured;
		scott->Attach(Content::LoadModel("gccontent/Mushroom"));
		scott->tag = "Mushroom";
		scott->position = glm::vec3(150, 0, 170);
		Attach(scott);
	}

	{
		shared_ptr<GameComponent> scott = make_shared<GameComponent>();
		scott->drawMode = GameComponent::draw_modes::textured;
		scott->Attach(Content::LoadModel("gccontent/Mushroom"));
		scott->tag = "Mushroom";
		scott->position = glm::vec3(300, 0, 100);
		Attach(scott);
	}

	{
		shared_ptr<GameComponent> scott = make_shared<GameComponent>();
		scott->drawMode = GameComponent::draw_modes::textured;
		scott->Attach(Content::LoadModel("gccontent/Mushroom"));
		scott->tag = "Mushroom";
		scott->position = glm::vec3(-300, 0, -100);
		Attach(scott);
	}

	{
		shared_ptr<GameComponent> scott = make_shared<GameComponent>();
		scott->drawMode = GameComponent::draw_modes::textured;
		scott->Attach(Content::LoadModel("gccontent/Mushroom"));
		scott->tag = "Mushroom";
		scott->position = glm::vec3(-200, 0, 300);
		Attach(scott);
	}

	shared_ptr<GameComponent> cb = make_shared<GameComponent>();
	cb->drawMode = GameComponent::draw_modes::textured;
	cb->Attach(Content::LoadModel("gccontent/chalkboard"));
	cb->position = glm::vec3(0, 3, 0);
	Attach(cb);

	shared_ptr<GameComponent> skySphere = make_shared<GameComponent>();
	skySphere->drawMode = GameComponent::draw_modes::textured;
	skySphere->Attach(Content::LoadModel("gccontent/skysphere"));
	skySphere->position = glm::vec3(0, 0, 0);
	Attach(skySphere);


	ResetScene();

	if (!Game::Initialise()) {
		return false;
	}

	soundSystem->PlaySoundW("gccontent/escape", glm::vec3(0), true);
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

	if (keyState[SDL_SCANCODE_F1])
	{
		if (! lastPressed)
		{
			camFollowing = !camFollowing;
			lastPressed = true;
		}	
	}
	else
	{
		lastPressed = false;
	}

	
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

	if (camFollowing)
	{
		camera->GetController()->position = camera->position = flyThing->position;
		camera->orientation = flyThing->orientation * camera->GetController()->orientation;
		camera->RecalculateVectors();
		camera->combinedLook = camera->look; 
		camera->view = glm::lookAt(
			camera->GetController()->position
			, camera->position + camera->look
			, camera->up
			);
	}

	if (high)
	{
		PrintText("Its full of stars!!");
	}

	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != Game::Instance()->children.end())
	{	
		bool deleted = false;
		if ((*it)->tag == "Mushroom")
		{
			float dist = glm::length((*it)->position - camera->position);
			PrintFloat("Dist: ", dist);
			
			if (dist < findDist)
			{
				high = true;
				
				camFollowing = true;
				findDist = 100.0f;
				it = children.erase(it);
				deleted = true;
				soundSystem->PlaySound("gccontent/mushroom", camera->position);
			}
		}
		if (! deleted)
		{
			++ it;
		}
	}

	if (high)
	{
		if (removable.size() > 0 )
		{
			for (int i = 0 ; i < removable.size() ; i ++)
			{
				children.remove(removable[i]);
			}
			removable.clear();
			soundSystem->PlaySound("gccontent/escapekick", camera->position);
		}
	}
}

void VRGame::Cleanup()
{
	Game::Cleanup();
}
