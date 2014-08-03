#pragma once
#include "GameComponent.h"
#include <vector>

namespace BGE
{
	class VectorDrawer :
		public GameComponent
	{
	private:
		glm::vec3 myScale; // scale will get updated from the parents scale, so I use this variable to remember my scale
	public:
		VectorDrawer(glm::vec3 scale = glm::vec3(5,5,5));
		~VectorDrawer(void);

		std::vector<glm::vec3> lines;
		std::vector<glm::vec3> colours;
		
		bool Initialise();
		void Draw();
	};
}
