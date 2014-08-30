#pragma once
#include <btBulletDynamicsCommon.h>
#include "GameComponent.h"

using namespace std;

namespace BGE
{
	class KinematicMotionState: public btMotionState 
	{
	public:
		KinematicMotionState(shared_ptr<GameComponent> owner);
		~KinematicMotionState(void);

		void getWorldTransform(btTransform &worldTrans) const;
		void setWorldTransform(const btTransform &worldTrans);

		shared_ptr<GameComponent> owner;
	};
}
