#pragma once
#include "Game.h"
#include "PhysicsController.h"
#include "PhysicsFactory.h"
#include <btBulletDynamicsCommon.h>

namespace BGE
{
	class PhysicsGame1 :
		public Game
	{
	private:
		
	public:
		PhysicsGame1(void);
		~PhysicsGame1(void);
		bool Initialise();
		void Update(float timeDelta);
		void Cleanup();
		void CreateWall();		
	};
}
