#include "Lab6.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"
#include "FountainEffect.h"

using namespace BGE;

Lab6::Lab6(void)
{
	elapsed = 10000;
	ySpeed = 5.0f;
}


Lab6::~Lab6(void)
{
}

bool Lab6::Initialise()
{	
	
	riftEnabled = false;
	fullscreen = false;
	
	// 500 in the constructor indicates the number of particles in the effect. 
	// You may need to compile in release mode or reduce the number of particles to get an acceptable framerate
	//shared_ptr<FountainEffect> centFountain = make_shared<FountainEffect>(500);
	//centFountain->position.x = centFountain->position.y = 20;
	//centFountain->position.y = FOUNTAIN_HEIGHT;
	//centFountain->diffuse = glm::vec3(1,1,0);

	//Attach(centFountain);

	// make a circle of fountains

	// Create a GameComponent and attach the Buddha model
	// Look at Lab5 for an example of how to do this
	// Use the buddha member variable to hold the GameComponent	
	// Set the scale to be 10, 10,10

	// Now create buddhaFountain0 and buddha fountain 1
	// You can control the colour by changing the diffuse member
	
	fountainTheta = 0.0f; 
	for (int i = 0 ; i < NUM_FOUNTAINS ; i ++)
	{
		fountainTheta = ((glm::pi<float>() * 2.0f) / NUM_FOUNTAINS) * i;
		shared_ptr<FountainEffect> fountain = make_shared<FountainEffect>(500);
		if (i % 2 == 0)
		{
			fountain->diffuse = glm::vec3(1,0,0);
		}
		else
		{
			fountain->diffuse = glm::vec3(0,1,0);
		}

		fountain->position.x = glm::sin(fountainTheta) * FOUNTAIN_RADIUS;
		fountain->position.z = - glm::cos(fountainTheta) * FOUNTAIN_RADIUS;
		fountain->position.y = FOUNTAIN_HEIGHT;
		fountains.push_back(fountain);
		Attach(fountain);
	}
	fountainTheta = 0.0f;

	hud = false;
	

	Game::Initialise();

	camera->GetController()->position = glm::vec3(0, 4, 80);
	return true;
}



void Lab6::Update(float timeDelta)
{		
	for (int i = 0 ; i < fountains.size() ; i ++)
	{
		if (i % 2 == 0)
		{
			fountains[i]->position.y = FOUNTAIN_HEIGHT + (glm::sin(fountainTheta) * FOUNTAIN_HEIGHT);
		}
		else
		{
			fountains[i]->position.y = FOUNTAIN_HEIGHT - (glm::sin(fountainTheta) * FOUNTAIN_HEIGHT);
		}
	}

	// Play with the scale using the sin function until you achieve the effect of Buddha breathing

	
	//
	if (fountainTheta >= glm::pi<float>() * 2.0f)
	{
		fountainTheta = 0.0f;
	}

	// Update buddhaFountan0 and buddhaFountain1 so that they follow a circle around the outside the Buddha (use sin & cos)
	// buddhaFountan0 needs to go upwards and buddhaFountan1 needs to go downwards
	// When they reach the top or bottom they should reverse direction


	Game::Update(timeDelta);
}
