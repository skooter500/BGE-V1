#include "Lab5.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"
#include "FountainEffect.h"

using namespace BGE;

Lab5::Lab5(void)
{
	elapsed = 10000;
}


Lab5::~Lab5(void)
{
}

bool Lab5::Initialise()
{
	std::shared_ptr<GameComponent> ground = make_shared<Ground>();
	Attach(ground);	

	ship1 = make_shared<GameComponent>();
	ship1->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship1->transform->position = glm::vec3(-10, 2, -10);
	ship1->Attach(make_shared<VectorDrawer>());
	Attach(ship1);

	
	//ship2->drawMode = GameComponent::single_material;



	riftEnabled = false;
	fullscreen = false;
	width = 800;
	height = 600;

	// 500 in the constructor indicates the number of particles in the effect. 
	// You may need to compile in release mode or reduce the number of particles to get an acceptable framerate
	shared_ptr<FountainEffect> centFountain = make_shared<FountainEffect>(500);
	centFountain->transform->position.x = centFountain->transform->position.y = 0;
	centFountain->transform->position.x = centFountain->transform->position.y = 0;
	centFountain->transform->position.y = FOUNTAIN_HEIGHT;
	centFountain->transform->diffuse = glm::vec3(1,1,0);

	Attach(centFountain);

	// make a circle of fountains
	
	
	fountainTheta = 0.0f; 
	for (int i = 0 ; i < NUM_FOUNTAINS ; i ++)
	{
		fountainTheta = ((glm::pi<float>() * 2.0f) / NUM_FOUNTAINS) * i;
		shared_ptr<FountainEffect> fountain = make_shared<FountainEffect>(500);
		if (i % 2 == 0)
		{
			fountain->transform->diffuse = glm::vec3(1,0,0);
		}
		else
		{
			fountain->transform->diffuse = glm::vec3(0,1,0);
		}
		
		fountain->transform->position.x = glm::sin(fountainTheta) * FOUNTAIN_RADIUS;
		fountain->transform->position.z = - glm::cos(fountainTheta) * FOUNTAIN_RADIUS;
		fountain->transform->position.y = FOUNTAIN_HEIGHT;
		fountains.push_back(fountain);
		Attach(fountain);
	}
	fountainTheta = 0.0f;
	
	ship2 = make_shared<GameComponent>();
	ship2->Attach(Content::LoadModel("ferdelance", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship2->Attach(make_shared<VectorDrawer>());
	ship2->transform->diffuse= glm::vec3(1.0f,0.0f,0.0f);
	ship2->transform->specular = glm::vec3(1.2f, 1.2f, 1.2f);

	ship2->transform->position = glm::vec3(10, 2, -10);
	Attach(ship2);


	Game::Initialise();

	camera->transform->position = glm::vec3(0, 4, 20);
	return true;
}

void Lab5::Update(float timeDelta)
{	
	// Movement of ship2
	if (keyState[SDL_SCANCODE_UP])
	{
		ship2->transform->position += ship2->transform->look * speed * timeDelta;
	}
	if (keyState[SDL_SCANCODE_DOWN])
	{
		ship2->transform->position -= ship2->transform->look * speed * timeDelta;
	}
	if (keyState[SDL_SCANCODE_LEFT])
	{
		ship2->transform->Yaw(timeDelta * speed * speed);
	}
	if (keyState[SDL_SCANCODE_RIGHT])
	{
		ship2->transform->Yaw(-timeDelta * speed * speed);
	}
	
	for (int i = 0 ; i < fountains.size() ; i ++)
	{
		if (i % 2 == 0)
		{
			fountains[i]->transform->position.y = FOUNTAIN_HEIGHT + (glm::sin(fountainTheta) * FOUNTAIN_HEIGHT);
		}
		else
		{
			fountains[i]->transform->position.y = FOUNTAIN_HEIGHT - (glm::sin(fountainTheta) * FOUNTAIN_HEIGHT);
		}
		
	}
	fountainTheta += timeDelta;
	if (fountainTheta >= glm::pi<float>() * 2.0f)
	{
		fountainTheta = 0.0f;
	}

	Game::Update(timeDelta);

	float theta = 0.0f;
	glm::vec3 toShip2 = ship2->transform->position - ship1->transform->position;
	toShip2 = glm::normalize(toShip2);
	theta = glm::acos(glm::dot(Transform::basisLook, toShip2));
	//if (toShip2.x > 0)
	//{
	//	theta = - theta;
	//}

	ship1->transform->world = glm::translate(glm::mat4(1), ship1->transform->position) * glm::rotate(glm::mat4(1), glm::degrees(theta), glm::vec3(0,1,0));
}
