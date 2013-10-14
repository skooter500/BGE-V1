#pragma once
#include "Game.h"
#include "GameComponent.h"

using namespace std;

namespace BGE
{
	class Lab4 :
		public Game
	{
	public:
		Lab4(void);
		~Lab4(void);

		shared_ptr<GameComponent> ship1;
		shared_ptr<GameComponent> ship2;
		float elapsed;
		bool Initialise();
		void Update(float timeDelta);
	};
}

