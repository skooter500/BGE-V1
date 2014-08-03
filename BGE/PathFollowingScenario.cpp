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
	shared_ptr<GameComponent> fighter = make_shared<GameComponent>(true);
	fighter->tag = "Steerable";
	fighter->transform->scale = glm::vec3(4, 4, 4);
	fighter->transform->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	shared_ptr<SteeringController> fighterController = make_shared<SteeringController>();
	fighter->Attach(fighterController);
	fighterController->transform->position = fighter->transform->position = glm::vec3(-20, 50, 50);
	fighterController->TurnOffAll();
	fighterController->Initialise();

	fighter->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, Transform::basisUp)));
	game->Attach(fighter);

	// Now create the enemy
	shared_ptr<GameComponent> enemy = make_shared<GameComponent>(true);
	enemy->tag = "Steerable";
	enemy->transform->scale = glm::vec3(4, 4, 4);
	enemy->transform->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	shared_ptr<SteeringController> enemyController = make_shared<SteeringController>();
	enemy->Attach(enemyController);
	enemyController->transform->position = enemy->transform->position = glm::vec3(10, 50, 0);
	enemyController->targetPos = fighterController->transform->position + glm::vec3(-50, 0, -80);
	enemyController->TurnOffAll();
	enemyController->TurnOn(SteeringController::behaviour_type::arrive);
	this->leaderController = enemyController;
	enemy->Attach(Content::LoadModel("marimba", glm::rotate(glm::mat4(1), 180.0f, Transform::basisUp)));

	game->Attach(enemy);

	// Now set up the state machine for the fighter..
	shared_ptr<StateMachine> stateMachine = make_shared<StateMachine>();
	fighter->Attach(stateMachine);
	stateMachine->SwicthState(make_shared<IdleState>(stateMachine, enemyController));
	

	// Now set up the camera
	game->camFollower = make_shared<GameComponent>(true);

	shared_ptr<SteeringController> camController = make_shared<SteeringController>();
	game->camFollower->Attach(camController);
	camController->offset = glm::vec3(0, 4, 4);
	camController->leader = fighterController;
	camController->transform->position = game->camFollower->transform->position = fighter->transform->position + camController->offset;
	camController->TurnOffAll();
	camController->TurnOn(SteeringController::behaviour_type::offset_pursuit);
	camController->TurnOn(SteeringController::behaviour_type::sphere_constrain);
	game->camFollower->Attach(make_shared<VectorDrawer>(glm::vec3(5,5,5)));

	game->Attach(game->camFollower);
	
	game->camera->transform->position = camController->transform->position;
}
