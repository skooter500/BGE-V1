#pragma once
#include "GameComponent.h"
#include <vector>

using namespace std;

namespace BGE
{
	class Route :
		public GameComponent
	{
	private:
		int next;
	public:
		Route(void);
		~Route(void);

		void Draw();
		glm::vec3 NextWaypoint();
		void AdvanceToNext();
		bool IsLast();

		bool looped;
		bool draw;
		vector<glm::vec3> waypoints;
	};
}
