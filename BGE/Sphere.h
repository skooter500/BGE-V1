#pragma once
#include "gamecomponent.h"


namespace BGE
{
	class Sphere :
		public GameComponent
	{
	public:
		Sphere(float radius);
		~Sphere(void);
	};
}
