#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "FountainEffect.h"

#define NUM_FOUNTAINS 20
#define FOUNTAIN_RADIUS 100.0f
#define FOUNTAIN_HEIGHT 10.0f

using namespace std;

namespace BGE
{
	class Lab6 :
		public Game
	{
	public:
		Lab6(void);
		~Lab6(void);

		shared_ptr<GameComponent> buddha;
		shared_ptr<FountainEffect> buddhaFountain0;
		shared_ptr<FountainEffect> buddhaFountain1;
		float elapsed;
		float ySpeed;
		bool Initialise();
		void Update(float timeDelta);
		vector<shared_ptr<FountainEffect>> fountains;
		float fountainTheta;
	};
}

