#pragma once
#include "Scenario.h"

namespace BGE
{
	class ObstacleAvoidanceScenario :
		public Scenario
	{
	public:
		ObstacleAvoidanceScenario(void);
		~ObstacleAvoidanceScenario(void);

		string Description();
		void Initialise();

	};
}
