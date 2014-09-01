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
#include "Utils.h"

using namespace BGE;

PhysicsGame1::PhysicsGame1(void)
{
}

PhysicsGame1::~PhysicsGame1(void)
{
}

shared_ptr<PhysicsController> cyl;
std::shared_ptr<GameComponent> station;

bool PhysicsGame1::Initialise() 
{	
	physicsFactory->CreateGroundPhysics();
	physicsFactory->CreateCameraPhysics();	

	shared_ptr<PhysicsController> box1 = physicsFactory->CreateBox(1,1,4, glm::vec3(5, 5, 0), glm::quat()); 
	shared_ptr<PhysicsController> box2 = physicsFactory->CreateBox(1,1,4, glm::vec3(5, 5, 5), glm::quat()); 

	// A hinge
	btHingeConstraint * hinge = new btHingeConstraint(*box1->rigidBody, *box2->rigidBody, btVector3(0,0,2.5f),btVector3(0,0,-2.5f), btVector3(0,1,0), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);

	// Another hinge
	box1 = physicsFactory->CreateBox(6,1,2, glm::vec3(15, 5, 0), glm::quat());
	cyl = physicsFactory->CreateCylinder(2, 1, glm::vec3(15, 5, -5), glm::angleAxis(90.0f, glm::vec3(1,0,0)));
	hinge = new btHingeConstraint(*box1->rigidBody, *cyl->rigidBody, btVector3(0,0,-2),btVector3(0,2,0), btVector3(0,0,1), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);

	// A Ball and socket
	box1 = physicsFactory->CreateBox(1,1,4, glm::vec3(20, 5, 0), glm::quat()); 
	box2 = physicsFactory->CreateBox(1,1,4, glm::vec3(20, 5, 5), glm::quat()); 

	btPoint2PointConstraint * ptpConstraint = new btPoint2PointConstraint(*box1->rigidBody, *box2->rigidBody, btVector3(0,0,2.5f),btVector3(0,0,-2.5f));
	dynamicsWorld->addConstraint(ptpConstraint);

	// A Slider
	box1 = physicsFactory->CreateBox(1,1,4, glm::vec3(25, 5, 0), glm::quat()); 
	box2 = physicsFactory->CreateBox(1,1,4, glm::vec3(25, 5, 5), glm::quat()); 
	btTransform box1Transform;
	btTransform box2Transform;
	box1Transform.setIdentity();
	box2Transform.setIdentity();
	
	// You have to make the x axis rotate to the axis you want to slide
	box1Transform.setRotation(GLToBtQuat(glm::angleAxis(90.0f, glm::vec3(0,1,0))));
	box2Transform.setRotation(GLToBtQuat(glm::angleAxis(90.0f, glm::vec3(0,1,0))));
	
	btSliderConstraint * slider = new btSliderConstraint(*box1->rigidBody, *box2->rigidBody, box1Transform, box2Transform, true);
	dynamicsWorld->addConstraint(slider);

	if (!Game::Initialise()) {
		return false;
	}

	camera->transform->position = glm::vec3(0,10, 20);
	
	return true;
}

void BGE::PhysicsGame1::Update(float timeDelta)
{
	cyl->rigidBody->applyTorque(GLToBtVector(glm::vec3(0.0f,0.0f,1.0f)));

	Game::Update(timeDelta);
}

void BGE::PhysicsGame1::Cleanup()
{
	Game::Cleanup();
}
