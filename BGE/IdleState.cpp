#include "IdleState.h"
#include "AttackingState.h"
#include "SteeringControler.h"
#include "LineDrawer.h"

using namespace BGE;
using namespace std;

glm::vec3 IdleState::initialPos = glm::vec3(0);

IdleState::IdleState(shared_ptr<StateMachine> owner, shared_ptr<GameComponent> enemy):State(owner)
{
	this->enemy = enemy;
}

IdleState::~IdleState(void)
{
}

void IdleState::Enter()
{
	shared_ptr<SteeringController> fighterController = dynamic_pointer_cast<SteeringController> (owner->parent->GetController());
	if (initialPos == glm::vec3(0))
	{
		initialPos = fighterController->position;
	}
	fighterController->route->waypoints.push_back(initialPos);
	fighterController->route->waypoints.push_back(initialPos + glm::vec3(-50, 0, -80));
	fighterController->route->waypoints.push_back(initialPos + glm::vec3(0, 0, -160));
	fighterController->route->waypoints.push_back(initialPos + glm::vec3(50, 0, -80));
	fighterController->route->looped = true;
	fighterController->TurnOffAll();
	fighterController->TurnOn(SteeringController::behaviour_type::follow_path);
	fighterController->TurnOn(SteeringController::behaviour_type::obstacle_avoidance);
}

void IdleState::Exit()
{
	shared_ptr<SteeringController> fighter = dynamic_pointer_cast<SteeringController> (owner->parent->GetController());
	fighter->route->waypoints.clear();
}

void IdleState::Update(float timeDelta)
{
	float range = 30.0f;           
	shared_ptr<SteeringController> fighter = dynamic_pointer_cast<SteeringController> (owner->parent->GetController());

	// Can I see the leader?
	if (glm::length(enemy->position - fighter->position) < range)
	{
		owner->SwicthState(make_shared<AttackingState>(owner, enemy));
	}
}

string IdleState::Description()
{
	return "Idle";
}
