#include "KinematicMotionState.h"
#include "PhysicsController.h"

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
	worldTrans.setOrigin(PhysicsController::GLToBtVector(owner->position));
	worldTrans.setRotation(PhysicsController::GLToBtQuat(owner->orientation));
}

void KinematicMotionState::setWorldTransform(const btTransform &worldTrans)
{
	// Should never get called, because this body is kinematic??
}
