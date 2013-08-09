#include "PhysicsController.h"

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
	this->id = "Physics Component";
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

glm::vec3 PhysicsController::BtToGLVector(const btVector3 & v)
{
	return glm::vec3(v.getX(), v.getY(), v.getZ());
}

glm::quat PhysicsController::BtToGLQuat(const btQuaternion & q)
{
	return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
}

btVector3 PhysicsController::GLToBtVector(const glm::vec3 & v)
{
	return btVector3(v.x, v.y, v.z);
}

btQuaternion PhysicsController::GLToBtQuat(const glm::quat & q)
{
	return  btQuaternion(q.x, q.y, q.z, q.w);
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
