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
	ship1->position = glm::vec3(-10, 2, -10);
	ship1->Attach(make_shared<VectorDrawer>());
	Attach(ship1);

	
	
	riftEnabled = false;
	fullscreen = false;
	width = 800;
	height = 600;

	mass = 1.0f;
	ship1->velocity = glm::vec3(0,0,0);

	Game::Initialise();

	camera->GetController()->position = glm::vec3(0, 4, 20);
	return true;
}

void Lab8::Update(float timeDelta)
{	
	// Forces on ship2
	float newtons = 10.0f;
	float epsilon = glm::epsilon<float>();
	if (keyState[SDL_SCANCODE_UP])
	{
		//
	}
	
	// Now calculate the acceleration, new velocity and new position
	
	// Check if the velocity length is > epsilon and if so create the look vector from the velocity
	
	// Now check to see if the |look - basis| > epsilon
	// And if so calculate the quaternion
	// Apply damping
	// Reset the force accumulator
	Game::Update(timeDelta);

}
