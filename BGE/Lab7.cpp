#include "Lab7.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"
#include "FountainEffect.h"

using namespace BGE;

Lab7::Lab7(void)
{
	elapsed = 10000;
}


Lab7::~Lab7(void)
{
}

bool Lab7::Initialise()
{
	std::shared_ptr<GameComponent> ground = make_shared<Ground>();
	Attach(ground);	

	ship1 = make_shared<GameComponent>();
	ship1->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship1->position = glm::vec3(-10, 2, -10);
	ship1->Attach(make_shared<VectorDrawer>());
	Attach(ship1);

	ship2 = make_shared<GameComponent>();
	ship2->Attach(Content::LoadModel("ferdelance", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship2->Attach(make_shared<VectorDrawer>());
	ship2->diffuse= glm::vec3(1.0f,0.0f,0.0f);
	ship2->specular = glm::vec3(1.2f, 1.2f, 1.2f);

	ship2->position = glm::vec3(10, 2, -10);
	Attach(ship2);

	riftEnabled = false;
	fullscreen = false;
	width = 800;
	height = 600;

	slerping = false;
	t = 0.0f;

	Game::Initialise();

	camera->GetController()->position = glm::vec3(0, 4, 20);
	return true;
}

void Lab7::Update(float timeDelta)
{	
	// Movement of ship2
	if (keyState[SDL_SCANCODE_UP])
	{
		ship2->Walk(speed * timeDelta);
	}
	if (keyState[SDL_SCANCODE_DOWN])
	{
		ship2->Walk(-speed * timeDelta);
	}
	if (keyState[SDL_SCANCODE_LEFT])
	{
		ship2->Yaw(timeDelta * speed * speed);
	}
	if (keyState[SDL_SCANCODE_RIGHT])
	{
		ship2->Yaw(-timeDelta * speed * speed);
	}

	if (keyState[SDL_SCANCODE_O])
	{
		ship2->Fly(timeDelta * speed);
	}

	if (keyState[SDL_SCANCODE_L])
	{
		ship2->Fly(-timeDelta * speed);
	}

	if (keyState[SDL_SCANCODE_SPACE] && ! slerping)
	{
		slerping = true;
		fromQuaternion = ship1->orientation;

		glm::vec3 toShip2 = ship2->position - ship1->position;
		toShip2 = glm::normalize(toShip2);
		glm::vec3 axis = glm::cross(GameComponent::basisLook, toShip2);
		axis = glm::normalize(axis);
		float theta = glm::acos(glm::dot(toShip2, GameComponent::basisLook));
		toQuaternion = glm::angleAxis(glm::degrees(theta), axis);
	}

	if (slerping)
	{
		ship1->orientation = glm::mix(fromQuaternion, toQuaternion, t);
		t += timeDelta;
		if (t > 1.0f)
		{
			t = 0.0f;
			slerping = false;
		}
	}


	// Put code for ship1 here!!!
	/*
	// Solution to part 1
	glm::vec3 toShip2 = ship2->position - ship1->position;
	toShip2 = glm::normalize(toShip2);
	glm::vec3 axis = glm::cross(GameComponent::basisLook, toShip2);
	axis = glm::normalize(axis);
	float theta = glm::acos(glm::dot(toShip2, GameComponent::basisLook));
	ship1->orientation = glm::angleAxis(glm::degrees(theta), axis);
	*/
	// End code for ship 1	
	Game::Update(timeDelta);

}
