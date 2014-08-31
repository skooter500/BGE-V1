#pragma once
#include "GameComponent.h"
#include <iostream> 


namespace BGE
{
	class Cylinder :
		public GameComponent
	{
	public:
		Cylinder(float radius, float height);
		~Cylinder(void);

		bool Initialise();
		void Draw();
	};
}
