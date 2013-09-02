#pragma once
#include "GameComponent.h"
#include <list>

using namespace std;

namespace BGE
{
	class Route :
		public GameComponent
	{
	private:
		list<glm::vec3>::iterator next;
	public:
		Route(void);
		~Route(void);

		void Draw();
		glm::vec3 NextWaypoint();
		void AdvanceToNext();
		bool IsLast();

		bool looped;
		bool draw;
		list<glm::vec3> waypoints;
	};
}
