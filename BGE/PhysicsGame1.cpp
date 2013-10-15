#include "PhysicsGame1.h"
#include "PhysicsController.h"
#include "Sphere.h"
#include "PhysicsCamera.h"
#include "Box.h"
#include "Cylinder.h"
#include "Steerable3DController.h"
#include "Ground.h"
#include "Content.h"
#include <btBulletDynamicsCommon.h>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/norm.hpp>
#include "VectorDrawer.h"

using namespace BGE;

PhysicsGame1::PhysicsGame1(void)
{
	physicsFactory = NULL;
	dynamicsWorld = NULL;
	broadphase = NULL;
	dispatcher = NULL;
	solver = NULL;
	fullscreen = false;

}

PhysicsGame1::~PhysicsGame1(void)
{
}


std::shared_ptr<GameComponent> station;

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
    dynamicsWorld->setGravity(btVector3(0,-9,0));

	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);

	physicsFactory->CreateGroundPhysics();
	physicsFactory->CreateCameraPhysics();
	
	physicsFactory->CreateWall(glm::vec3(-20,0,20), 5, 5);
	// Now some constraints
	shared_ptr<PhysicsController> box1 = physicsFactory->CreateBox(1,1,4, glm::vec3(5, 5, 0), glm::quat()); 
	shared_ptr<PhysicsController> box2 = physicsFactory->CreateBox(1,1,4, glm::vec3(5, 5, 5), glm::quat()); 

	// A hinge
	btHingeConstraint * hinge = new btHingeConstraint(*box1->rigidBody, *box2->rigidBody, btVector3(0,0,2.5f),btVector3(0,0,-2.5f), btVector3(0,1,0), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);

	box1 = physicsFactory->CreateBox(1,1,4, glm::vec3(10, 5, 0), glm::quat()); 
	box2 = physicsFactory->CreateBox(1,1,4, glm::vec3(10, 5, 5), glm::quat());

	physicsFactory->CreateCylinder(10, 3, glm::vec3(0, 20, 0), glm::quat());

	std::shared_ptr<GameComponent> ship = make_shared<GameComponent>();
	ship->ambient = glm::vec3(0.2f, 0.2, 0.2f);
	ship->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	std::shared_ptr<Model> model = Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp));	
	std::shared_ptr<GameComponent> steerable = make_shared<Steerable3DController>(model);
	steerable->position = glm::vec3(20, 5, -20);
	std::shared_ptr<VectorDrawer> vectorDrawer = make_shared<VectorDrawer>();
	vectorDrawer->scale = glm::vec3(5,5,10);
	ship->Attach(steerable);
	ship->Attach(model);
	ship->Attach(vectorDrawer);
	Attach(ship);

	// Create a hierarchy
	station = make_shared<GameComponent>();
	station->worldMode = world_modes::from_self;
	station->ambient = glm::vec3(0.2f, 0.2, 0.2f);
	station->specular = glm::vec3(0,0,0);
	station->scale = glm::vec3(2,2,2);
	std::shared_ptr<Model> cmodel = Content::LoadModel("coriolis", glm::rotate(glm::mat4(1), 90.0f, GameComponent::basisUp));	
	station->Attach(cmodel);
	station->Attach(make_shared<VectorDrawer>(glm::vec3(7,7,7)));
	station->position = glm::vec3(40, 5, -20);
	Attach(station);

	// Add a child to the station and update by including the parent's world transform
	std::shared_ptr<GameComponent> ship1 = make_shared<GameComponent>();
	ship1->worldMode = world_modes::from_self_with_parent;
	ship1->ambient = glm::vec3(0.2f, 0.2, 0.2f);
	ship1->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	std::shared_ptr<Model> ana = Content::LoadModel("anaconda", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp));	
	ship1->Attach(ana);
	ship1->position = glm::vec3(0, 0, -10);
	station->Attach(ship1);

	physicsFactory->CreateVehicle(glm::vec3(0,10,-30));
	if (!Game::Initialise()) {
		return false;
	}

	return true;
}

void BGE::PhysicsGame1::Update(float timeDelta)
{
	dynamicsWorld->stepSimulation(timeDelta,100);
	station->Yaw(timeDelta * 20.0f);
	Game::Update(timeDelta);
}

void BGE::PhysicsGame1::Cleanup()
{
	Game::Cleanup();
}
