#include "KinematicMotionState.h"
#include "PhysicsController.h"
#include "Utils.h"

using namespace BGE;

KinematicMotionState::KinematicMotionState(shared_ptr<GameComponent> owner)
{
	this->owner = owner;
}


KinematicMotionState::~KinematicMotionState(void)
{
}

void KinematicMotionState::getWorldTransform(btTransform &worldTrans) const
{
	worldTrans.setOrigin(GLToBtVector(owner->transform->position));
	worldTrans.setRotation(GLToBtQuat(owner->transform->orientation));
}

void KinematicMotionState::setWorldTransform(const btTransform &worldTrans)
{
	// Should never get called, because this body is kinematic??
}
