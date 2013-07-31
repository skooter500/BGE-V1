#include "PhysicsGame1.h"
#include "PhysicsComponent.h"
#include "Sphere.h"
#include "PhysicsCamera.h"
#include "Box.h"
#include "Ground.h"
#include <btBulletDynamicsCommon.h>

BGE::PhysicsGame1::PhysicsGame1(void)
{
}


BGE::PhysicsGame1::~PhysicsGame1(void)
{
}

void BGE::PhysicsGame1::CreateWall()
{
	float blockWidth = 5;
	float blockHeight = 5;
	float blockDepth = 5;

	float startX = 0;
	float z = -20;
	float gap = 2;

	btCollisionShape * boxShape = new btBoxShape(btVector3(blockWidth, blockHeight, blockDepth) * 0.5);
	btScalar mass = 1;
	btVector3 fallInertia(0,0,0);
	boxShape->calculateLocalInertia(mass,fallInertia);

	for (int w = 0 ; w < 5 ; w ++)
	{
		for (int h = 0 ; h < 5 ; h ++)	
		{
			float x = startX + ((blockWidth + 2) * w);
			float y = ((blockHeight + gap) / 2.0f) + ((blockHeight + gap) * h);
			Box * box = new Box(blockWidth, blockHeight, blockDepth);
			box->position = glm::vec3(x, y, z);
			AddChild(box);
	
			btDefaultMotionState * boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
			
			btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,boxMotionState, boxShape, fallInertia);
			btRigidBody * boxBody = new btRigidBody(fallRigidBodyCI);
			dynamicsWorld->addRigidBody(boxBody);

			BGE::PhysicsComponent * boxComponent = new PhysicsComponent(boxShape, boxBody);
			boxComponent->id = "A box";
			boxComponent->scale = box->scale;
			box->AddChild(boxComponent);
		}
	}
}


bool BGE::PhysicsGame1::Initialise() 
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
    dynamicsWorld->setGravity(btVector3(0,-5,0));

	// Now add some stuff to the world...

	// The ground
	Ground * ground = new Ground();
	
	btCollisionShape * groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
    btDefaultMotionState * groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));

    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);
	groundRigidBody->setUserPointer(ground);
	PhysicsComponent * groundComponent = new PhysicsComponent(groundShape, groundRigidBody);
	groundComponent->id = "Ground";
	ground->AddChild(groundComponent);
	
	SetGround(ground);

	CreateWall();

	Sphere * sphere = new Sphere(1);
	AddChild(sphere);
	
	btDefaultMotionState * sphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,10,0)));
	btScalar mass = 1;
    btVector3 fallInertia(0,0,0);
	btCollisionShape * sphereShape = new btSphereShape(1);
	
    sphereShape->calculateLocalInertia(mass,fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,sphereMotionState, sphereShape, fallInertia);
    btRigidBody * sphereBody = new btRigidBody(fallRigidBodyCI);
    dynamicsWorld->addRigidBody(sphereBody);

	PhysicsComponent * sphereComponent = new PhysicsComponent(sphereShape, sphereBody);	
	sphere->AddChild(sphereComponent);

	// Now add physics to the camera
	btCollisionShape * cameraCyl = new btCylinderShape(btVector3(0.5f, 5.0f, 2.5f));
	cameraCyl->calculateLocalInertia(mass, fallInertia);
	PhysicsCamera * physicsCamera = new PhysicsCamera();
	
	Camera * camera = Game::Instance()->GetCamera();
	camera->AddChild(physicsCamera);

	btRigidBody::btRigidBodyConstructionInfo cameraCI(mass,physicsCamera, cameraCyl, fallInertia);  
	btRigidBody * cameraBody = new btRigidBody(cameraCI);
	physicsCamera->SetPhysicsStuff(cameraCyl, cameraBody);
	cameraBody->setCollisionFlags(cameraBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	cameraBody->setActivationState(DISABLE_DEACTIVATION);
    
	dynamicsWorld->addRigidBody(cameraBody);
	
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
	delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}
