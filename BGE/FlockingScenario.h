#pragma once
#include "Scenario.h"
namespace BGE
{
	class FlockingScenario :
		public Scenario
	{
	public:
		FlockingScenario(void);
		~FlockingScenario(void);

		string Description();
		void Initialise();

	};
}

