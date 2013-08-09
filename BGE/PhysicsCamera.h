#pragma once
#include "Camera.h"
#include "PhysicsController.h"
#include <btBulletDynamicsCommon.h>

namespace BGE

{
	class PhysicsCamera :
		public PhysicsController, public btMotionState 
	{
	private:
		PhysicsController * pickedUp;
	public:
		PhysicsCamera();
		~PhysicsCamera(void);

		//bool Initialise();
		void PhysicsCamera::Update(float timeDelta);

		void getWorldTransform(btTransform &worldTrans) const;
		void setWorldTransform(const btTransform &worldTrans);
		float elapsed;
		float fireRate;
	};
}
