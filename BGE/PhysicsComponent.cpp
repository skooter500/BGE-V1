#include "PhysicsComponent.h"

using namespace BGE;

PhysicsComponent::PhysicsComponent()
{
	shape = NULL;
	rigidBody = NULL;
	motionState = NULL;
}

PhysicsComponent::PhysicsComponent(btCollisionShape * shape, btRigidBody * rigidBody, btMotionState * motionState)
{
	this->shape = shape;
	this->rigidBody = rigidBody;
	if (this->rigidBody)
	{
		this->rigidBody->setUserPointer(this);
	}
	this->motionState = motionState;
	attachedToParent = false;
}

void PhysicsComponent::SetPhysicsStuff(btCollisionShape * shape, btRigidBody * rigidBody, btMotionState * motionState)
{
	this->shape = shape;
	this->rigidBody = rigidBody;
	this->rigidBody->setUserPointer(this);
	this->motionState = motionState;
}
PhysicsComponent::~PhysicsComponent(void)
{
}

glm::vec3 PhysicsComponent::BtToGLVector(const btVector3 & v)
{
	return glm::vec3(v.getX(), v.getY(), v.getZ());
}

glm::quat PhysicsComponent::BtToGLQuat(const btQuaternion & q)
{
	return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
}

btVector3 PhysicsComponent::GLToBtVector(const glm::vec3 & v)
{
	return btVector3(v.x, v.y, v.z);
}

btQuaternion PhysicsComponent::GLToBtQuat(const glm::quat & q)
{
	return  btQuaternion(q.x, q.y, q.z, q.w);
}

void PhysicsComponent::Update(float timeDelta)
{

	btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
	position = BtToGLVector(trans.getOrigin());
	orientation = BtToGLQuat(trans.getRotation());
	
	// Calculate the world transform 
	GameComponent::Update(timeDelta);

	// Update the parent...
	parent->position = position;
	parent->orientation = orientation;
	parent->world = world;
}

void PhysicsComponent::Cleanup()
{
	SafeDelete(rigidBody);
	SafeDelete(shape);
	SafeDelete(motionState);
	GameComponent::Cleanup();

}
