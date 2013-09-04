#include "PathFollowingScenario.h"
#include "SteeringGame.h"
#include "Params.h"
#include "Content.h"
#include "VectorDrawer.h"

using namespace BGE;

PathFollowingScenario::PathFollowingScenario()
{
}


PathFollowingScenario::~PathFollowingScenario()
{
}

string PathFollowingScenario::Description()
{
	return string("Path Following Scenario");
}

void PathFollowingScenario::Initialise()
{
	SteeringGame * game = (SteeringGame *) Game::Instance();
	Params::Load("default");

	shared_ptr<Ground> ground = make_shared<Ground>();
	game->Attach(ground);
	game->ground= ground;

	shared_ptr<GameComponent> fighter = make_shared<GameComponent>();
	fighter->scale = glm::vec3(4, 4, 4);
	shared_ptr<SteeringController> fighterController = make_shared<SteeringController>();
	fighterController->position = fighter->position = glm::vec3(10, 50, 0);
	fighterController->TurnOffAll();
	
	glm::vec3 initialPos = fighterController->position;
	fighterController->route->waypoints.push_back(initialPos);
	fighterController->route->waypoints.push_back(initialPos + glm::vec3(-150, 15, -180));
	fighterController->route->waypoints.push_back(initialPos + glm::vec3(0, -10, -260));
	fighterController->route->waypoints.push_back(initialPos + glm::vec3(150, 5, -180));
	fighterController->route->looped = true;
	fighterController->TurnOffAll();
	fighterController->TurnOn(SteeringController::behaviour_type::follow_path);
	fighterController->TurnOn(SteeringController::behaviour_type::obstacle_avoidance);

	fighter->Attach(fighterController);
	fighter->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp)));
	game->Attach(fighter);

	
	game->camFollower = make_shared<GameComponent>();
	shared_ptr<SteeringController> camController = make_shared<SteeringController>();
	camController->offset = glm::vec3(0, 4, 4);
	camController->leader = fighterController;
	camController->position = game->camFollower->position = fighter->position + camController->offset;
	camController->TurnOffAll();
	camController->TurnOn(SteeringController::behaviour_type::offset_pursuit);
	camController->TurnOn(SteeringController::behaviour_type::sphere_constrain);
	//game->camFollower->Attach(make_shared<VectorDrawer>(glm::vec3(5,5,5)));

	game->Attach(game->camFollower);
	game->camFollower->Attach(camController);
	
	
	game->camera->position = camController->position;
}
