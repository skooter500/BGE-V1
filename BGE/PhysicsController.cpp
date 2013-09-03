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
	this->tag = "Physics Component";
	this->motionState = motionState;
	worldMode = world_modes::to_parent;
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
	position = BtToGLVector(trans.getOrigin());
	orientation = BtToGLQuat(trans.getRotation());
	
	// Calculate the world transform 
	GameComponent::Update(timeDelta);
}

void PhysicsController::Cleanup()
{
	GameComponent::Cleanup();

}
