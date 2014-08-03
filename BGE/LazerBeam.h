#pragma once
#include "GameComponent.h"

namespace BGE
{
	class LazerBeam :
		public GameComponent
	{
	public:
		LazerBeam(void);
		~LazerBeam(void);
		void Draw();
		void Update(float timeDelta);
	};
}

