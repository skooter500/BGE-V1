#include "FountainEffect.h"
#include "Utils.h"

using namespace BGE;

FountainEffect::FountainEffect(void):numParticles(1000)
{
	
}

FountainEffect::FountainEffect(int numParticles)
{
	this->numParticles = numParticles;
	
}


FountainEffect::~FountainEffect(void)
{
}

bool FountainEffect::Initialise()
{
	transform->diffuse = glm::vec3(0,0,1);
	for (int i = 0 ; i < numParticles ; i ++)
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
	p.position = transform->position; // Start the particle at the ParticleSystem position
	// Give the particle a clamped random velocity
	p.velocity = glm::vec3(RandomClamped(-radius, radius), RandomClamped(), RandomClamped(-radius, radius)) * 10.0f;
	
	// Make the velocity always point upwards
	p.velocity.y = glm::abs<float>(p.velocity.y);
	
	// Give the particle a random colour
	p.diffuse.r = transform->diffuse.r * RandomClamped(0, 1);
	p.diffuse.g = transform->diffuse.g * RandomClamped(0, 1);
	p.diffuse.b = transform->diffuse.b * RandomClamped(0, 1);
	p.age = 0;
	p.alive = true;
	// Random sizes also
	p.size = RandomClamped(10, 50);
}

void FountainEffect::UpdateParticle(float timeDelta, Particle & p)
{
	static glm::vec3 gravity = glm::vec3(0, -9.8, 0);
	
	p.velocity += gravity * timeDelta;
	p.position += p.velocity * timeDelta;

	// Fade the alpha as we approach the ground
	float fadeHeight = 5;
	
	p.diffuse.a = glm::clamp<float>(p.position.y / fadeHeight, 0.0f, 1.0f);
	if (p.position.y < 0)
	{
		InitParticle(p);
	}
}

void FountainEffect::Update(float timeDelta)
{
	ParticleEffect::Update(timeDelta);
}

