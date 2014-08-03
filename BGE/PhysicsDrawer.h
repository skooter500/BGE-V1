#pragma once
#include <btBulletDynamicsCommon.h>
class PhysicsDrawer :
	public btIDebugDraw
{
public:
	PhysicsDrawer(void);
	~PhysicsDrawer(void);
};

