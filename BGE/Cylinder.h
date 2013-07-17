#pragma once
#include "GameComponent.h"


namespace BGE
{
	class Cylinder :
		public GameComponent
	{
	public:
		Cylinder(float height, float radius);
		~Cylinder(void);
	};
}
