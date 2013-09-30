#include "SteeringGame.h"
#include "Params.h"
#include "SteeringControler.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "Scenario.h"
#include "FlockingScenario.h"
#include "PathFollowingScenario.h"
#include "ObstacleAvoidanceScenario.h"

using namespace BGE;

SteeringGame::SteeringGame(void)
{
	lastPressed = false;
	camFollowing = false;
	scenarios.push_back(make_shared<PathFollowingScenario>());
	scenarios.push_back(make_shared<ObstacleAvoidanceScenario>());
	scenarios.push_back(make_shared<FlockingScenario>());
	
	currentScenario = 0;
	elapsed = 10000;
}


SteeringGame::~SteeringGame(void)
{
}

bool SteeringGame::Initialise()
{
	Params::Load("default");

	riftEnabled = false;
	fullscreen = true;

	scenarios[currentScenario]->Initialise();

	return Game::Initialise();
}

void SteeringGame::Reset()
{
	list<shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		if ((*it)->tag == "Steerable" || (*it)->tag == "Route" || (*it)->tag == "Obstacle")
		{
			it = children.erase(it);
		}
		else
		{
			++ it;
		}
	}
	if (ground != nullptr)
	{
		children.remove(ground);
		ground = nullptr;
	}
}

void SteeringGame::Update(float timeDelta)
{
	static float multiplier = 1.0f;

	float timeToPass = 1.0f;

	PrintText("Press F1 to toggle camera following");
	PrintText(string("Current Scenario:" + scenarios[currentScenario]->Description()));

	if (keyState[SDL_SCANCODE_F1])
	{
		if (! lastPressed)
		{
			camFollowing = !camFollowing;
			lastPressed = true;
		}	
	}
	else
	{
		lastPressed = false;
	}

	PrintText("Press number keys to cycle Scenarios");

	for (int i = 0 ; i < scenarios.size() ; i ++)
	{
		if (keyState[SDL_SCANCODE_1 + i] && (elapsed > timeToPass))
		{
			currentScenario = i;
			Reset();
			scenarios[currentScenario]->Initialise();
			if (ground != nullptr)
			{
				ground->Initialise();
			}
			Content::InitialiseModels();
			elapsed = 0.0f;
		}
	}
	elapsed += timeDelta;
	
	PrintText("Press O to decrease speed");
	PrintText("Press P to increase speed");

	if (keyState[SDL_SCANCODE_O])
	{
		multiplier -= timeDelta;
	}
	if (keyState[SDL_SCANCODE_P])
	{
		multiplier += timeDelta;
	}
	scenarios[currentScenario]->Update(timeDelta * multiplier);

	Game::Update(timeDelta  * multiplier);

	if (camFollowing)
	{
		camera->GetController()->position = camFollower->position;
		camera->orientation = camFollower->orientation * camera->GetController()->orientation;
		camera->RecalculateVectors();
		camera->view = glm::lookAt(
			camera->position
			, camera->position + camera->look
			, camera->up
			);

	}
}
