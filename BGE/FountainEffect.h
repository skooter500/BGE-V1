#pragma once
#include "ParticleEffect.h"

namespace BGE
{
	class FountainEffect :
		public ParticleEffect
	{
	public:
		FountainEffect(void);
		FountainEffect(int numParticles);
		~FountainEffect(void);

		bool Initialise();
		void Update(float timeDelta);
		void InitParticle(Particle & particle);
		void UpdateParticle(float timeDelta, Particle & particle);

		int numParticles;
	};
}

