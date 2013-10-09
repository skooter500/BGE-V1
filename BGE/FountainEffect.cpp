#include "FountainEffect.h"
#include "Utils.h"

using namespace BGE;

FountainEffect::FountainEffect(void)
{
}


FountainEffect::~FountainEffect(void)
{
}

bool FountainEffect::Initialise()
{
	for (int i = 0 ; i < 1000 ; i ++)
	{
		Particle p;
		InitParticle(p);
		particles.push_back(p);
	}
	return ParticleEffect::Initialise();
}

void FountainEffect::InitParticle(Particle & p)
{
	float radius = 0.25f;
	p.position = position;

	p.velocity = glm::vec3(RandomClamped(-radius, radius), RandomClamped(), RandomClamped(-radius, radius)) * 20.0f;
	p.velocity.y = glm::abs<float>(p.velocity.y);
	p.colour = glm::vec4(0,0,RandomClamped(0, 1),1);
	p.age = 0;
	p.alive = true;
	p.size = RandomClamped(10, 50);
	p.lifetime = 10.0f;
}

void FountainEffect::UpdateParticle(float timeDelta, Particle & p)
{
	static glm::vec3 gravity = glm::vec3(0, -9.8, 0);
	
	p.velocity += gravity * timeDelta;
	p.position += p.velocity * timeDelta;

	// Fade the alpha as we approach the ground
	float fadeHeight = 5;
	
	p.colour.a = glm::clamp<float>(p.position.y / fadeHeight, 0.0f, 1.0f);
	if (p.position.y < 0)
	{
		InitParticle(p);
	}
}

void FountainEffect::Update(float timeDelta)
{
	ParticleEffect::Update(timeDelta);
}

