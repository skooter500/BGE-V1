#include "PhysicsFactory.h"
#include "Game.h"
#include "Sphere.h"
#include "Box.h"
#include "Cylinder.h"
#include "Ground.h"
#include "PhysicsCamera.h"

using namespace BGE;

PhysicsFactory::PhysicsFactory(btDiscreteDynamicsWorld * dynamicsWorld)
{
	this->dynamicsWorld = dynamicsWorld;
}


PhysicsFactory::~PhysicsFactory(void)
{
}

shared_ptr<PhysicsComponent> PhysicsFactory::CreateSphere(float radius, glm::vec3 pos, glm::quat quat)
{
	shared_ptr<GameComponent> sphere (new Sphere(radius));
	Game::Instance()->AddChild(sphere);

	btDefaultMotionState * sphereMotionState = new btDefaultMotionState(btTransform(PhysicsComponent::GLToBtQuat(quat)
		,PhysicsComponent::GLToBtVector(pos)));	

	btScalar mass = 1;
	btVector3 sphereInertia(0,0,0);
	btCollisionShape * sphereShape = new btSphereShape(radius);

	sphereShape->calculateLocalInertia(mass,sphereInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,sphereMotionState, sphereShape, sphereInertia);
	btRigidBody * sphereBody = new btRigidBody(fallRigidBodyCI);
	dynamicsWorld->addRigidBody(sphereBody);

	shared_ptr<PhysicsComponent> sphereComponent (new PhysicsComponent(sphereShape, sphereBody, sphereMotionState));	
	sphere->AddChild(sphereComponent);
	sphereComponent->id = "Sphere";	
	return sphereComponent;
}


shared_ptr<PhysicsComponent> PhysicsFactory::CreateBox(float width, float height, float depth, glm::vec3 pos, glm::quat quat)
{
	// Create the shape
	btCollisionShape * boxShape = new btBoxShape(btVector3(width, height, depth) * 0.5);
	btScalar mass = 1;
	btVector3 boxInertia(0,0,0);
	boxShape->calculateLocalInertia(mass,boxInertia);

	// This is a container for the box model
	shared_ptr<Box> box = make_shared<Box>(Box(width, height, depth));
	box->position = pos;
	Game::Instance()->AddChild(box);

	// Create the rigid body
	btDefaultMotionState * boxMotionState = new btDefaultMotionState(btTransform(PhysicsComponent::GLToBtQuat(quat)
		,PhysicsComponent::GLToBtVector(pos)));			
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,  boxMotionState, boxShape, boxInertia);
	btRigidBody * boxBody = new btRigidBody(fallRigidBodyCI);
	boxBody->setFriction(567);
	dynamicsWorld->addRigidBody(boxBody);

	// Create the physics component and add it to the box
	shared_ptr<PhysicsComponent> boxComponent = make_shared<PhysicsComponent>(PhysicsComponent(boxShape, boxBody, boxMotionState));
	boxComponent->id = "Box";
	boxBody->setUserPointer(boxComponent.get());
	boxComponent->scale = box->scale;
	box->AddChild(boxComponent);

	return boxComponent;
}

shared_ptr<PhysicsComponent> PhysicsFactory::CreateCylinder(float radius, float height, glm::vec3 pos, glm::quat quat)
{
	// Create the shape
	btCollisionShape * shape = new btCylinderShape(btVector3(radius, height * 0.5f, radius));
	btScalar mass = 1;
	btVector3 inertia(0,0,0);
	shape->calculateLocalInertia(mass,inertia);

	// This is a container for the box model
	shared_ptr<GameComponent> cyl = make_shared<GameComponent>(Cylinder(radius, height));
	cyl->position = pos;
	Game::Instance()->AddChild(cyl);

	// Create the rigid body
	btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(PhysicsComponent::GLToBtQuat(quat)
		,PhysicsComponent::GLToBtVector(pos)));			
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,  motionState, shape, inertia);
	btRigidBody * body = new btRigidBody(rigidBodyCI);
	dynamicsWorld->addRigidBody(body);

	// Create the physics component and add it to the box
	shared_ptr<PhysicsComponent> component = make_shared<PhysicsComponent>(PhysicsComponent(shape, body, motionState));
	component->id = "Cylinder";
	component->scale = cyl->scale;
	cyl->AddChild(component);

	return component;
}

shared_ptr<PhysicsComponent> PhysicsFactory::CreateCameraPhysics()
{
	btVector3 inertia;
	// Now add physics to the camera
	btCollisionShape * cameraCyl = new btCylinderShape(btVector3(0.5f, 5.0f, 2.5f));
	cameraCyl->calculateLocalInertia(1, inertia);
	shared_ptr<PhysicsCamera> physicsCamera (new PhysicsCamera());

	shared_ptr<Camera> camera = Game::Instance()->GetCamera();
	camera->AddChild(physicsCamera);

	btRigidBody::btRigidBodyConstructionInfo cameraCI(10,physicsCamera.get(), cameraCyl, inertia);  
	btRigidBody * cameraBody = new btRigidBody(cameraCI);
	physicsCamera->SetPhysicsStuff(cameraCyl, cameraBody, physicsCamera.get());
	cameraBody->setCollisionFlags(cameraBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	cameraBody->setActivationState(DISABLE_DEACTIVATION);

	dynamicsWorld->addRigidBody(cameraBody);
	return physicsCamera;
}

shared_ptr<PhysicsComponent> PhysicsFactory::CreateVehicle(glm::vec3 position)
{
	float width = 15;
	float height = 2;
	float length = 5;
	float wheelWidth = 1;
	float wheelRadius = 2;
	float wheelOffset = 2.0f;

	shared_ptr<PhysicsComponent> chassis = CreateBox(width, height, length, position, glm::quat());

	shared_ptr<PhysicsComponent> wheel;
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

shared_ptr<PhysicsComponent> PhysicsFactory::CreateGroundPhysics()
{
	shared_ptr<Ground> ground (new Ground());

	btCollisionShape * groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
	btDefaultMotionState * groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));

	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setFriction(100);
	dynamicsWorld->addRigidBody(groundRigidBody);
	groundRigidBody->setUserPointer(ground.get());
	shared_ptr<PhysicsComponent> groundComponent (new PhysicsComponent(groundShape, groundRigidBody, groundMotionState));
	groundComponent->id = "Ground";
	ground->AddChild(groundComponent);	
	Game::Instance()->SetGround(ground);
	return groundComponent;
}
