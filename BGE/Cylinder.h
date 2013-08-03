#pragma once
#include "GameComponent.h"


namespace BGE
{
	class Cylinder :
		public GameComponent
	{
	public:
		Cylinder(float radius, float height);
		~Cylinder(void);
	};
}
