#include "Buddha.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"
#include "FountainEffect.h"

using namespace BGE;

Buddha::Buddha(void)
{
	elapsed = 10000;
	riftEnabled = false;
	fullscreen = false;
}


Buddha::~Buddha(void)
{
}

bool Buddha::Initialise()
{	
	
	soundSystem->PlaySound("Buddha", glm::vec3(0,0,0));
	riftEnabled = false;
	fullscreen = true;
	/*width = 800;
	height = 600;
*/
	// 500 in the constructor indicates the number of particles in the effect. 
	// You may need to compile in release mode or reduce the number of particles to get an acceptable framerate
	//shared_ptr<FountainEffect> centFountain = make_shared<FountainEffect>(500);
	//centFountain->position.x = centFountain->position.y = 20;
	//centFountain->position.y = FOUNTAIN_HEIGHT;
	//centFountain->diffuse = glm::vec3(1,1,0);

	//Attach(centFountain);

	// make a circle of fountains

	buddha = make_shared<GameComponent>();
	buddha->Attach(Content::LoadModel("buddha"));
	buddha->position = glm::vec3(0,0,0);
	buddha->scale = glm::vec3(10,10,10);
	buddha->diffuse = glm::vec3(1.2f, 1.2f, 1.2f);
	Attach(buddha);

	buddhaFountain0 = make_shared<FountainEffect>(500);
	buddhaFountain0->position = glm::vec3(-30, 0, 0);
	buddhaFountain0->diffuse = glm::vec3(1,1, 0);
	Attach(buddhaFountain0);

	buddhaFountain1 = make_shared<FountainEffect>(500);
	buddhaFountain1->position = glm::vec3(-30, 50, 0);
	buddhaFountain1->diffuse = glm::vec3(1,1, 0);
	Attach(buddhaFountain1);

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

float ySpeed = 5.0f;

void Buddha::Update(float timeDelta)
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
	float scale = 30.0f + (glm::sin(fountainTheta) / 3.0f);
	buddha->scale = glm::vec3(scale, scale, scale);
	fountainTheta += timeDelta;
	if (fountainTheta >= glm::pi<float>() * 2.0f)
	{
		fountainTheta = 0.0f;
	}

	buddhaFountain0->position.x = glm::sin(fountainTheta) * 30;
	buddhaFountain0->position.z = - glm::cos(fountainTheta) * 30;
	buddhaFountain0->position.y -= timeDelta * ySpeed;
	if (buddhaFountain0->position.y > 50)
	{
		ySpeed = -ySpeed;
		buddhaFountain0->position.y = 50;
	}

	if (buddhaFountain0->position.y < 0)
	{
		ySpeed = -ySpeed;
		buddhaFountain0->position.y = 0;
	}

	buddhaFountain1->position.x = glm::sin(fountainTheta) * -30;
	buddhaFountain1->position.z = glm::cos(fountainTheta) * 30;
	buddhaFountain1->position.y += timeDelta * ySpeed;
	if (buddhaFountain1->position.y > 50)
	{
		ySpeed = -ySpeed;
		buddhaFountain1->position.y = 50;
	}

	if (buddhaFountain1->position.y < 0)
	{
		ySpeed = -ySpeed;
		buddhaFountain1->position.y = 0;
	}


	Game::Update(timeDelta);
}
