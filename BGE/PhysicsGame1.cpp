#include "PhysicsGame1.h"
#include "PhysicsComponent.h"
#include "Sphere.h"
#include "PhysicsCamera.h"
#include "Box.h"
#include "Cylinder.h"
#include "ThreeDSteerable.h"
#include "Ground.h"
#include "Person.h"
#include "Content.h"
#include <btBulletDynamicsCommon.h>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/norm.hpp>

using namespace BGE;

PhysicsGame1::PhysicsGame1(void)
{
	physicsFactory = NULL;
	dynamicsWorld = NULL;
	broadphase = NULL;
	dispatcher = NULL;
	solver = NULL;
}

PhysicsGame1::~PhysicsGame1(void)
{
}

void PhysicsGame1::CreateWall()
{
	float blockWidth = 5;
	float blockHeight = 5;
	float blockDepth = 5;

	float start = -40;
	float z = -40;
	float gap = 1;

	for (int w = 0 ; w < 5 ; w ++)
	{
		for (int h = 0 ; h < 5 ; h ++)	
		{
			float x = start + ((blockWidth + 2) * w);
			float y = ((blockHeight + gap) / 2.0f) + ((blockHeight + gap) * h);
			physicsFactory->CreateBox(blockWidth, blockHeight, blockDepth, glm::vec3(x, y, z), glm::quat());
		}
	}
}

bool PhysicsGame1::Initialise() 
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
    dynamicsWorld->setGravity(btVector3(0,-9.8,0));

	physicsFactory = new PhysicsFactory(dynamicsWorld);

	physicsFactory->CreateGroundPhysics();
	physicsFactory->CreateCameraPhysics();
	//shared_ptr<Person> person = make_shared<Person>();
	//AddChild(person);

	//CreateWall();
	//glm::quat q =  glm::angleAxis(glm::half_pi<float>(), glm::vec3(1, 0, 0)); 
	////CreateCylinder(5, 1, glm::vec3(5,15,5), q);

	//// Now some constraints
	//shared_ptr<PhysicsComponent> box1 = physicsFactory->CreateBox(1,1,4, glm::vec3(5, 5, 0), glm::quat()); 
	//shared_ptr<PhysicsComponent> box2 = physicsFactory->CreateBox(1,1,4, glm::vec3(5, 5, 5), glm::quat()); 

	//// A hinge
	//btHingeConstraint * hinge = new btHingeConstraint(*box1->rigidBody, *box2->rigidBody, btVector3(0,0,2.5f),btVector3(0,0,-2.5f), btVector3(0,1,0), btVector3(0,1,0), true);
	//dynamicsWorld->addConstraint(hinge);

	//box1 = physicsFactory->CreateBox(1,1,4, glm::vec3(10, 5, 0), glm::quat()); 
	//box2 = physicsFactory->CreateBox(1,1,4, glm::vec3(10, 5, 5), glm::quat());

	std::shared_ptr<GameComponent> ship = make_shared<GameComponent>();
	ship->ambient = glm::vec3(0.2f, 0.2, 0.2f);
	ship->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	std::shared_ptr<Model> model = Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp));	
	std::shared_ptr<GameComponent> steerable = make_shared<ThreeDSteerable>(model);
	steerable->position = glm::vec3(20, 5, -20);
	ship->AddChild(model);
	ship->AddChild(steerable);
	AddChild(ship);

	//physicsFactory->CreateVehicle(glm::vec3(0,10,-30));
	fullscreen = false;
	console = true;
	if (!Game::Initialise()) {
		return false;
	}

	return true;
}

void BGE::PhysicsGame1::Update(float timeDelta)
{
	dynamicsWorld->stepSimulation(timeDelta,100);
	Game::Update(timeDelta);
}

void BGE::PhysicsGame1::Cleanup()
{
	
	Game::Cleanup();
}
