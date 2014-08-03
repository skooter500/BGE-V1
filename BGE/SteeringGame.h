#pragma once
#include "Game.h"
#include "SteeringControler.h"
#include "GameComponent.h"
#include <vector>
#include "Scenario.h"

using namespace std;

namespace BGE
{
	class SteeringGame :
		public Game
	{
	public:
		SteeringGame(void);
		~SteeringGame(void);

		bool Initialise();
		void Update(float timeDelta);
		void Reset();

		shared_ptr<GameComponent> camFollower;
		
		bool lastPressed;
		bool camFollowing;

		vector<shared_ptr<Scenario>> scenarios;
		int currentScenario;

		float elapsed;
	};
}
