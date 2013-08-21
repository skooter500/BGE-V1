#pragma once
#include "GameComponent.h"
#include <vector>


namespace BGE
{
	class VectorDrawer :
		public GameComponent
	{
	public:
		VectorDrawer(void);
		~VectorDrawer(void);

		std::vector<glm::vec3> lines;
		std::vector<glm::vec3> colours;

		void Draw();
	};
}
