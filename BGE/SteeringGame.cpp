#include "SteeringGame.h"
#include "Params.h"
#include "SteeringControler.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "Scenario.h"
#include "FlockingScenario.h"
#include "PathFollowingScenario.h"

using namespace BGE;

SteeringGame::SteeringGame(void)
{
	lastPressed = false;
	camFollowing = false;
	scenarios.push_back(make_shared<FlockingScenario>());
	scenarios.push_back(make_shared<PathFollowingScenario>());
	currentScenario = 1;
}


SteeringGame::~SteeringGame(void)
{
}

bool SteeringGame::Initialise()
{
	Params::Load("default");

	riftEnabled = true;
	fullscreen = true;


	scenarios[currentScenario]->Initialise();

	return Game::Initialise();
}

void SteeringGame::Update(float timeDelta)
{
	static float multiplier = 1.0f;

	PrintText("Press F1 to toggle camera following");
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
