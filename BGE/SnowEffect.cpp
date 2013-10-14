#include "SnowEffect.h"
#include "Utils.h"

using namespace BGE;

SnowEffect::SnowEffect(void)
{
	textureName = "snowflake";
}


SnowEffect::~SnowEffect(void)
{
}

bool SnowEffect::Initialise()
{
	for (int i = 0 ; i < 1000 ; i ++)
	{
		Particle p;
		InitParticle(p);
		p.position.y = RandomClamped(0, 1000);
		particles.push_back(p);
	}
	return ParticleEffect::Initialise();
}

void SnowEffect::InitParticle(Particle & p)
{
	float range = 1000.0f;
	float radius = 0.1f;
	p.position = glm::vec3(RandomClamped(-range, range), range, RandomClamped(-range, range));

	p.velocity = glm::vec3(RandomClamped(-radius, radius), RandomClamped(- radius, -radius * 5.0f), RandomClamped(-radius, radius));
	p.diffuse = glm::vec4(1,1,1,1);
	p.age = 0;
	p.alive = true;
	p.size = RandomClamped(10, 20);
	p.lifetime = 10.0f;
}

void SnowEffect::UpdateParticle(float timeDelta, Particle & p)
{
	static glm::vec3 gravity = glm::vec3(0, -9.8, 0);

	p.velocity += gravity * timeDelta;
	p.position += p.velocity * timeDelta;

	// Fade the alpha as we approach the ground
	float fadeHeight = 20;

	p.diffuse.a = glm::clamp<float>(p.position.y / fadeHeight, 0.0f, 1.0f);
	if (p.position.y < 0)
	{
		InitParticle(p);
	}
}
