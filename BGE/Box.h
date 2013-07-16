#pragma once
#include "gamecomponent.h"

namespace BGE
{
	class Box :
		public GameComponent
	{
	public:
		Box(void);
		~Box(void);
		bool Initialize();
		void Update(float timeDelta);

		

	};
}
