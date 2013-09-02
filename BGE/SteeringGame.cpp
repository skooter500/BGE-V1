#include "SteeringGame.h"
#include "Params.h"
#include "SteeringControler.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "Scenario.h"

using namespace BGE;

SteeringGame::SteeringGame(void)
{
}


SteeringGame::~SteeringGame(void)
{
}

bool SteeringGame::Initialise()
{
	Params::Load("default");

	riftEnabled = false;
	fullscreen = false;

	//Attach(make_shared<Ground>());

	Scenario::Setup();

	/*shared_ptr<GameComponent> ship = make_shared<GameComponent>();
	ship->worldMode = world_modes::from_child;
	ship->id = "steerable";
	shipSteering = make_shared<SteeringControler>(); 
	shipSteering->TurnOn(SteeringControler::behaviour_type::seek);
	shipSteering->position = glm::vec3(0, 5, -5);
	shipSteering->targetPos = glm::vec3(20, 5, -20);
	ship->Attach(shipSteering);
	ship->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp)));
	ship->Attach(make_shared<VectorDrawer>(glm::vec3(5,5,5)));
	Attach(ship);*/
	return Game::Initialise();
}

void SteeringGame::Update(float timeDelta)
{
	if (SDL_GetMouseState(NULL, NULL) && SDL_BUTTON(3))
	{
		shipSteering->targetPos = camera->position + camera->look * 50.0f;
	}

	Game::Update(timeDelta);
}
