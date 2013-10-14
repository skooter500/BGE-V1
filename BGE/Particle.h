#pragma once
#include "GameComponent.h"

class Particle
{
public:
	Particle(void);
	~Particle(void);

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 diffuse;
	float size;
	float lifetime;
	float age;
	bool alive;
};