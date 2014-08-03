#pragma once
#include "Camera.h"
#include "PhysicsController.h"
#include <btBulletDynamicsCommon.h>
#include "PhysicsFactory.h"

namespace BGE

{
	class PhysicsCamera :
		public PhysicsController, public btMotionState 
	{
	private:
		PhysicsController * pickedUp;
	public:
		PhysicsCamera(PhysicsFactory * physicsFactory);
		~PhysicsCamera(void);

		//bool Initialise();
		void PhysicsCamera::Update(float timeDelta);

		void getWorldTransform(btTransform &worldTrans) const;
		void setWorldTransform(const btTransform &worldTrans);
		PhysicsFactory * physicsFactory;
		float elapsed;
		float fireRate;
	};
}
