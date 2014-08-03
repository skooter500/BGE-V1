#include "Lab8.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"
#include "FountainEffect.h"

using namespace BGE;

Lab8::Lab8(void)
{
	elapsed = 10000;
}


Lab8::~Lab8(void)
{
}

bool Lab8::Initialise()
{
	std::shared_ptr<GameComponent> ground = make_shared<Ground>();
	Attach(ground);	

	ship1 = make_shared<GameComponent>();
	ship1->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship1->transform->position = glm::vec3(-10, 2, -10);
	ship1->Attach(make_shared<VectorDrawer>());
	Attach(ship1);

	riftEnabled = false;
	fullscreen = false;
	width = 800;
	height = 600;

	mass = 1.0f;
	ship1->transform->velocity = glm::vec3(0,0,0);

	Game::Initialise();

	camera->transform->position = glm::vec3(0, 4, 20);
	return true;
}

void Lab8::Update(float timeDelta)
{	
	// Forces on ship1
	float newtons = 10.0f;
	float epsilon = glm::epsilon<float>();
	if (keyState[SDL_SCANCODE_UP])
	{
		force += ship1->transform->look * newtons;
	}
	if (keyState[SDL_SCANCODE_DOWN])
	{
		force -= ship1->transform->look * newtons;
	}
	if (keyState[SDL_SCANCODE_LEFT])
	{
		force -= ship1->transform->right * newtons;
	}
	if (keyState[SDL_SCANCODE_RIGHT])
	{
		force += ship1->transform->right * newtons;
	}

	// Now calculate the acceleration, new velocity and new transform->position
	glm::vec3 accel = force / mass;
	ship1->transform->velocity += accel * timeDelta;
	ship1->transform->position += ship1->transform->velocity * timeDelta;
	// Check if the velocity length is > epsilon and if so create the look vector from the velocity
	if (glm::length(ship1->transform->velocity) > epsilon)
	{
		ship1->transform->look = glm::normalize(ship1->transform->velocity);		
	}
	// Now check to see if the |look - basis| > epsilon
	// And if so calculate the quaternion
	if (glm::length(ship1->transform->look - Transform::basisLook) > epsilon)
	{
		glm::vec3 axis = glm::cross(Transform::basisLook, ship1->transform->look);
		axis = glm::normalize(axis);
		float theta = glm::acos(glm::dot(ship1->transform->look, Transform::basisLook));
		ship1->transform->orientation = glm::angleAxis(glm::degrees(theta), axis);
	}
	// Apply damping
	ship1->transform->velocity *= 0.99f;
	// Reset the force accumulator
	force = glm::vec3(0,0,0);
	Game::Update(timeDelta);

}
