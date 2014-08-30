#pragma once
#include "GameComponent.h"


namespace BGE
{
	class Sphere :
		public GameComponent
	{
	public:
		Sphere(float radius);
		~Sphere(void);

		bool Initialise();
	};
}
