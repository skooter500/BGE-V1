#pragma once
#include "Scenario.h"
#include <string>

using namespace std;

namespace BGE
{
	class FlockingScenario :
		public Scenario
	{
	public:
		FlockingScenario(void);

		string Description();
		void Initialise();

	};
}

