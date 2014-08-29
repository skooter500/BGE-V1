#pragma once
#include "Game.h"
#include "PhysicsFactory.h"
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <iostream>
#include "Content.h"
#include "VectorDrawer.h"
#include "XBoxController.h"
#include "Steerable3DController.h"
#include "SteeringControler.h"
#include "Params.h"
#include "FountainEffect.h"
#include "Box.h"
#include "SnowEffect.h"

using namespace std;

namespace BGE
{
	class SceneGraphGame :
		public Game
	{
	private:		
		glm::vec3 NextPosition(float step, float steps);
		vector<glm::vec3> waypoints;
		shared_ptr<GameComponent> selfExample;
		shared_ptr<GameComponent> station;
		shared_ptr<GameComponent> ship1; 
	public:
		SceneGraphGame(void);
		~SceneGraphGame(void);
		bool Initialise();
		void Update(float timeDelta);
		void CreateScene();
	};
}

