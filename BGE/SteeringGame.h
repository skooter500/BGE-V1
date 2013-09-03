#pragma once
#include "Game.h"
#include "SteeringControler.h"
#include "GameComponent.h"

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

		shared_ptr<GameComponent> camFollower;
		bool lastPressed;
		bool camFollowing;
	};
}
