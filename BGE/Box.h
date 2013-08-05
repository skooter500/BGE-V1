#pragma once
#include "GameComponent.h"

namespace BGE
{
	class Box :
		public GameComponent
	{
	public:
		Box(float width, float height, float depth);
		Box();
		~Box(void);
		bool Initialize();
		void Update(float timeDelta);

		

	};
}
