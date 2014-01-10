#pragma once
#include "GameComponent.h"


namespace BGE
{
	class CapsuleShape : public GameComponent
	{
	public:
		CapsuleShape(float radius, float height);
		~CapsuleShape(void);
	};
}

