#pragma once
#include "Game.h"
#include "SteeringControler.h"

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

		shared_ptr<SteeringControler> shipSteering;

	};
}
