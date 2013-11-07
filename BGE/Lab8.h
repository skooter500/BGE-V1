#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "FountainEffect.h"

using namespace std;

namespace BGE
{
	class Lab8 :
		public Game
	{
	public:
		Lab8(void);
		~Lab8(void);

		shared_ptr<GameComponent> ship1;
		float elapsed;
		bool Initialise();
		void Update(float timeDelta);

		glm::vec3 force;
		float mass;
	};
}

