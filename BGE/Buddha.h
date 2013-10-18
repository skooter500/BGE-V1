#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "FountainEffect.h"

#define NUM_FOUNTAINS 20
#define FOUNTAIN_RADIUS 40.0f
#define FOUNTAIN_HEIGHT 10.0f

using namespace std;

namespace BGE
{
	class Buddha :
		public Game
	{
	public:
		Buddha(void);
		~Buddha(void);

		shared_ptr<GameComponent> buddha;
		float elapsed;
		bool Initialise();
		void Update(float timeDelta);
		vector<shared_ptr<FountainEffect>> fountains;
		float fountainTheta;
	};
}

