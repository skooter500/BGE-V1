#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "FountainEffect.h"

using namespace std;

namespace BGE
{
	class Lab7 :
		public Game
	{
	public:
		Lab7(void);
		~Lab7(void);

		shared_ptr<GameComponent> ship1;
		shared_ptr<GameComponent> ship2;
		float elapsed;
		bool Initialise();
		void Update(float timeDelta);
		bool slerping;
		glm::quat fromQuaternion;
		glm::quat toQuaternion;
		float t;
	};
}

