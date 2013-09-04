#pragma once
#include <string>

using namespace std;

namespace BGE
{
	class Scenario
	{
	public:
		Scenario(void);
		~Scenario(void);

		virtual string Description() = 0;
		virtual void Initialise() = 0;
		virtual void Update(float timeDelta);
	};
}
