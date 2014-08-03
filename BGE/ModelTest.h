#pragma once
#include "Game.h"
#include "Model.h"

namespace BGE
{
	class ModelTest :public Game
	{
		private:
		BGE::Model * model;
	public:
		ModelTest(void);
		~ModelTest(void);

		bool Initialise();		
		void Update(float timeDelta);
	};
}

