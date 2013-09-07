#include "PathFollowingScenario.h"
#include "SteeringGame.h"
#include "Params.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "State.h"
#include "IdleState.h"

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
	
	// Create the fighter
	shared_ptr<GameComponent> fighter = make_shared<GameComponent>();
	fighter->tag = "Steerable";
	fighter->scale = glm::vec3(4, 4, 4);
	shared_ptr<SteeringController> fighterController = make_shared<SteeringController>();
	fighterController->position = fighter->position = glm::vec3(-20, 50, 50);
	fighterController->TurnOffAll();
	fighterController->Initialise();

	fighter->Attach(fighterController);
	fighter->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp)));
	game->Attach(fighter);

	// Now create the enemy
	shared_ptr<GameComponent> enemy = make_shared<GameComponent>();
	enemy->tag = "Steerable";
	enemy->scale = glm::vec3(4, 4, 4);
	shared_ptr<SteeringController> enemyController = make_shared<SteeringController>();
	enemyController->position = enemy->position = glm::vec3(10, 50, 0);
	enemyController->targetPos = fighterController->position + glm::vec3(-50, 0, -80);
	enemyController->TurnOffAll();
	enemyController->TurnOn(SteeringController::behaviour_type::arrive);
	this->leaderController = enemyController;
	enemy->Attach(enemyController);
	enemy->Attach(Content::LoadModel("python", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp)));

	game->Attach(enemy);

	// Now set up the state machine for the fighter..
	shared_ptr<StateMachine> stateMachine = make_shared<StateMachine>();
	fighter->Attach(stateMachine);
	stateMachine->SwicthState(make_shared<IdleState>(stateMachine, enemyController));
	

	// Now set up the camera
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
