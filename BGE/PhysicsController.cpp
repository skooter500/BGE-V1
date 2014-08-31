#include "PhysicsController.h"
#include "Utils.h"
#include "Game.h"
using namespace BGE;

PhysicsController::PhysicsController()
{
	shape = nullptr;
	rigidBody = nullptr;
	motionState = nullptr;
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
	Game::Instance()->dynamicsWorld->removeRigidBody(rigidBody);
	SAFE_DELETE(rigidBody);
	SAFE_DELETE(motionState);
}

void PhysicsController::Update(float timeDelta)
{
	btTransform trans;
	btMotionState * ms = rigidBody->getMotionState();
    ms->getWorldTransform(trans);
	transform->position = BtToGLVector(trans.getOrigin());
	transform->orientation = BtToGLQuat(trans.getRotation());
	
	// Calculate the world transform 
	GameComponent::Update(timeDelta);
}

void PhysicsController::Cleanup()
{
	GameComponent::Cleanup();

}
