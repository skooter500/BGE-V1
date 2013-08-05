#include "KinematicMotionState.h"
#include "PhysicsComponent.h"

using namespace BGE;

KinematicMotionState::KinematicMotionState(GameComponent * owner)
{
	this->owner = owner;
}


KinematicMotionState::~KinematicMotionState(void)
{
}

void KinematicMotionState::getWorldTransform(btTransform &worldTrans) const
{
	worldTrans.setOrigin(PhysicsComponent::GLToBtVector(owner->position));
	worldTrans.setRotation(PhysicsComponent::GLToBtQuat(owner->orientation));
}

void KinematicMotionState::setWorldTransform(const btTransform &worldTrans)
{
	// Should never get called, because this body is kinematic??
}
