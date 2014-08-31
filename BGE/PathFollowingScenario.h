#pragma once
#include "Scenario.h"
namespace BGE
{
	class PathFollowingScenario:public Scenario
	{
	public:
		PathFollowingScenario(void);

		string Description();
		void Initialise();
	};
}
