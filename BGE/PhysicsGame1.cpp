#include "PhysicsGame1.h"
#include "PhysicsComponent.h"
#include "Sphere.h"
#include "PhysicsCamera.h"
#include "Box.h"
#include "Cylinder.h"
#include "Ship.h"
#include "Ground.h"
#include "Person.h"
#include <btBulletDynamicsCommon.h>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/norm.hpp>

using namespace BGE;

PhysicsGame1::PhysicsGame1(void)
{
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
	float x = 60;
	float gap = 1;

	for (int w = 0 ; w < 100 ; w ++)
	{
		for (int h = 0 ; h < 10 ; h ++)	
		{
			float z = start + ((blockWidth + 2) * w);
			float y = ((blockHeight + gap) / 2.0f) + ((blockHeight + gap) * h);
			CreateBox(blockWidth, blockHeight, blockDepth, glm::vec3(x, y, z), glm::quat());
		}
	}
}

PhysicsComponent * PhysicsGame1::CreateSphere(float radius, glm::vec3 pos, glm::quat quat)
{
	Sphere * sphere = new Sphere(radius);
	AddChild(sphere);
	
	btDefaultMotionState * sphereMotionState = new btDefaultMotionState(btTransform(PhysicsComponent::GLToBtQuat(quat)
		,PhysicsComponent::GLToBtVector(pos)));	

	btScalar mass = 1;
    btVector3 sphereInertia(0,0,0);
	btCollisionShape * sphereShape = new btSphereShape(radius);
	
    sphereShape->calculateLocalInertia(mass,sphereInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,sphereMotionState, sphereShape, sphereInertia);
    btRigidBody * sphereBody = new btRigidBody(fallRigidBodyCI);
    dynamicsWorld->addRigidBody(sphereBody);

	PhysicsComponent * sphereComponent = new PhysicsComponent(sphereShape, sphereBody, sphereMotionState);	
	sphere->AddChild(sphereComponent);
	sphereComponent->id = "Sphere";	
	return sphereComponent;
}


PhysicsComponent * PhysicsGame1::CreateBox(float width, float height, float depth, glm::vec3 pos, glm::quat quat)
{
	// Create the shape
	btCollisionShape * boxShape = new btBoxShape(btVector3(width, height, depth) * 0.5);
	btScalar mass = 1;
	btVector3 boxInertia(0,0,0);
	boxShape->calculateLocalInertia(mass,boxInertia);

	// This is a container for the box model
	Box * box = new Box(width, height, depth);
	box->position = pos;
	AddChild(box);
	
	// Create the rigid body
	btDefaultMotionState * boxMotionState = new btDefaultMotionState(btTransform(PhysicsComponent::GLToBtQuat(quat)
		,PhysicsComponent::GLToBtVector(pos)));			
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,  boxMotionState, boxShape, boxInertia);
	btRigidBody * boxBody = new btRigidBody(fallRigidBodyCI);
	boxBody->setFriction(500);
	dynamicsWorld->addRigidBody(boxBody);

	// Create the physics component and add it to the box
	BGE::PhysicsComponent * boxComponent = new PhysicsComponent(boxShape, boxBody, boxMotionState);
	boxComponent->id = "Box";
	boxComponent->scale = box->scale;
	box->AddChild(boxComponent);

	return boxComponent;
}

PhysicsComponent * PhysicsGame1::CreateCylinder(float radius, float height, glm::vec3 pos, glm::quat quat)
{
	// Create the shape
	btCollisionShape * shape = new btCylinderShape(btVector3(radius, height * 0.5f, radius));
	btScalar mass = 1;
	btVector3 inertia(0,0,0);
	shape->calculateLocalInertia(mass,inertia);

	// This is a container for the box model
	Cylinder * cyl = new Cylinder(radius, height);
	cyl->position = pos;
	AddChild(cyl);
	
	// Create the rigid body
	btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(PhysicsComponent::GLToBtQuat(quat)
		,PhysicsComponent::GLToBtVector(pos)));			
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,  motionState, shape, inertia);
	btRigidBody * body = new btRigidBody(rigidBodyCI);
	dynamicsWorld->addRigidBody(body);

	// Create the physics component and add it to the box
	BGE::PhysicsComponent * component = new PhysicsComponent(shape, body, motionState);
	component->id = "Cylinder";
	component->scale = cyl->scale;
	cyl->AddChild(component);

	return component;
}

PhysicsComponent * PhysicsGame1::CreateCameraPhysics()
{
	btVector3 inertia;
	// Now add physics to the camera
	btCollisionShape * cameraCyl = new btCylinderShape(btVector3(0.5f, 5.0f, 2.5f));
	cameraCyl->calculateLocalInertia(1, inertia);
	PhysicsCamera * physicsCamera = new PhysicsCamera();
	
	Camera * camera = Game::Instance()->GetCamera();
	camera->AddChild(physicsCamera);

	btRigidBody::btRigidBodyConstructionInfo cameraCI(10,physicsCamera, cameraCyl, inertia);  
	btRigidBody * cameraBody = new btRigidBody(cameraCI);
	physicsCamera->SetPhysicsStuff(cameraCyl, cameraBody, physicsCamera);
	cameraBody->setCollisionFlags(cameraBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	cameraBody->setActivationState(DISABLE_DEACTIVATION);
    
	dynamicsWorld->addRigidBody(cameraBody);
	return physicsCamera;
	
}

PhysicsComponent * PhysicsGame1::CreateVehicle(glm::vec3 position)
{
    float width = 15;
    float height = 2;
    float length = 5;
    float wheelWidth = 1;
    float wheelRadius = 2;
	float wheelOffset = 2.0f;

    PhysicsComponent * chassis = CreateBox(width, height, length, position, glm::quat());
            
    PhysicsComponent * wheel;
	glm::quat q =  glm::angleAxis(glm::half_pi<float>(), glm::vec3(1, 0, 0));

	glm::vec3 offset;
	btHingeConstraint * hinge;
	
	offset = glm::vec3(- (width / 2 - wheelRadius), 0, - (length / 2 + wheelOffset));
    wheel = CreateCylinder(wheelRadius, wheelWidth, position + offset, q);	 
	hinge = new btHingeConstraint(* chassis->rigidBody, * wheel->rigidBody, PhysicsComponent::GLToBtVector(offset),btVector3(0,0, 0), btVector3(0,0,1), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);
            
	offset = glm::vec3(+ (width / 2 - wheelRadius), 0, - (length / 2 + wheelOffset));
    wheel = CreateCylinder(wheelRadius, wheelWidth, glm::vec3(position.x + (width / 2) - wheelRadius, position.y, position.z - (length / 2) - wheelWidth), q);
    hinge = new btHingeConstraint(* chassis->rigidBody, * wheel->rigidBody, PhysicsComponent::GLToBtVector(offset),btVector3(0,0, 0), btVector3(0,0,1), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);

	offset = glm::vec3(- (width / 2 - wheelRadius), 0, + (length / 2 + wheelOffset));
    wheel = CreateCylinder(wheelRadius, wheelWidth, position + offset, q);	 
	hinge = new btHingeConstraint(* chassis->rigidBody, * wheel->rigidBody, PhysicsComponent::GLToBtVector(offset),btVector3(0,0, 0), btVector3(0,0,1), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);
    
	offset = glm::vec3(+ (width / 2 - wheelRadius), 0, + (length / 2 + wheelOffset));
    wheel = CreateCylinder(wheelRadius, wheelWidth, position + offset, q);	 
	hinge = new btHingeConstraint(* chassis->rigidBody, * wheel->rigidBody, PhysicsComponent::GLToBtVector(offset),btVector3(0,0, 0), btVector3(0,0,1), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);
            
	

  	return chassis;
}

PhysicsComponent * PhysicsGame1::CreateGroundPhysics()
{
	Ground * ground = new Ground();
	
	btCollisionShape * groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
    btDefaultMotionState * groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));

    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setFriction(100);
    dynamicsWorld->addRigidBody(groundRigidBody);
	groundRigidBody->setUserPointer(ground);
	PhysicsComponent * groundComponent = new PhysicsComponent(groundShape, groundRigidBody, groundMotionState);
	groundComponent->id = "Ground";
	ground->AddChild(groundComponent);	
	SetGround(ground);
	return groundComponent;
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

	CreateGroundPhysics();
	CreateCameraPhysics();
	Person * person = new Person();
	AddChild(person);


	/*
	CreateWall();
	glm::quat q =  glm::angleAxis(glm::half_pi<float>(), glm::vec3(1, 0, 0)); 
	//CreateCylinder(5, 1, glm::vec3(5,15,5), q);

	// Now some constraints
	PhysicsComponent * box1 = CreateBox(1,1,4, glm::vec3(5, 5, 0), glm::quat()); 
	PhysicsComponent * box2 = CreateBox(1,1,4, glm::vec3(5, 5, 5), glm::quat()); 

	// A hinge
	btHingeConstraint * hinge = new btHingeConstraint(*box1->rigidBody, *box2->rigidBody, btVector3(0,0,2.5f),btVector3(0,0,-2.5f), btVector3(0,1,0), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);

	box1 = CreateBox(1,1,4, glm::vec3(10, 5, 0), glm::quat()); 
	box2 = CreateBox(1,1,4, glm::vec3(10, 5, 5), glm::quat()); 

	CreateVehicle(glm::vec3(0,10,-30));
	
	Ship * ship = new Ship();
	ship->position = glm::vec3(5, 20, 5);
	AddChild(ship);
	*/
	
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
