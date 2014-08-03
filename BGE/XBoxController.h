#pragma once
#include "GameComponent.h"

namespace BGE
{
	class XBoxController :
		public GameComponent
	{
	public:
		XBoxController(bool hasTransform = false);
		~XBoxController(void);

		void Update(float TimeDelta);
		bool Initialise();
		bool disablePitch;
	};
}
