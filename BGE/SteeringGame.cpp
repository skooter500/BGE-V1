#include "SteeringGame.h"
#include "Params.h"
#include "SteeringControler.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "Scenario.h"

using namespace BGE;

SteeringGame::SteeringGame(void)
{
	lastPressed = false;
	camFollowing = false;
}


SteeringGame::~SteeringGame(void)
{
}

bool SteeringGame::Initialise()
{
	Params::Load("default");

	riftEnabled = false;
	fullscreen = false;


	Scenario::Setup();

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

	if (camFollowing)
	{
		camera->GetController()->position = camFollower->position;
		camera->GetController()->orientation = camFollower->orientation;
	}

	Game::Update(timeDelta  * multiplier);
}
