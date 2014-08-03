#include "PhysicsController.h"
#include "Utils.h"
using namespace BGE;

PhysicsController::PhysicsController()
{
	shape = NULL;
	rigidBody = NULL;
	motionState = NULL;
}

PhysicsController::PhysicsController(btCollisionShape * shape, btRigidBody * rigidBody, btMotionState * motionState)
{
	this->shape = shape;
	this->rigidBody = rigidBody;
	if (this->rigidBody)
	{
		this->rigidBody->setUserPointer(this);
	}
	this->tag = "PhysicsController";
	this->motionState = motionState;
}

void PhysicsController::SetPhysicsStuff(btCollisionShape * shape, btRigidBody * rigidBody, btMotionState * motionState)
{
	this->shape = shape;
	this->rigidBody = rigidBody;
	this->rigidBody->setUserPointer(this);
	this->motionState = motionState;
}
PhysicsController::~PhysicsController(void)
{
}

void PhysicsController::Update(float timeDelta)
{
	btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
	transform->position = BtToGLVector(trans.getOrigin());
	transform->orientation = BtToGLQuat(trans.getRotation());
	
	// Calculate the world transform 
	GameComponent::Update(timeDelta);
}

void PhysicsController::Cleanup()
{
	GameComponent::Cleanup();

}
