#include "AttackingState.h"
#include "SteeringControler.h"
#include "IdleState.h"
#include "LineDrawer.h"
#include "LazerBeam.h"
#include "Game.h"

using namespace BGE;
using namespace std;

AttackingState::AttackingState(shared_ptr<StateMachine> owner, shared_ptr<GameComponent> enemy):State(owner)
{
	this->enemy = enemy;
	timeShot = 0.25f;
}


AttackingState::~AttackingState(void)
{
}

void AttackingState::Enter()
{
	shared_ptr<SteeringController> fighterController = dynamic_pointer_cast<SteeringController> (owner->parent->GetController());

	fighterController->TurnOffAll();
	fighterController->TurnOn(SteeringController::behaviour_type::offset_pursuit);
	fighterController->TurnOn(SteeringController::behaviour_type::obstacle_avoidance);
	fighterController->offset = glm::vec3(0, 0, 5);
	fighterController->leader = enemy;
}

void AttackingState::Exit()
{
	//throw std::logic_error("The method or operation is not implemented.");
}

void AttackingState::Update(float timeDelta)
{
	float range = 30.0f;
	timeShot += timeDelta;
	float fov = glm::quarter_pi<float>();
	// Can I see the leader?
	shared_ptr<SteeringController> fighterController = dynamic_pointer_cast<SteeringController> (owner->parent->GetController());

	if (glm::length(enemy->position - fighterController->position) > range)
	{
		owner->SwicthState(make_shared<IdleState>(owner, enemy));
	}
	else
	{
		float angle;
		glm::vec3 toEnemy = (enemy->position - fighterController->position);
		toEnemy = glm::normalize(toEnemy);
		angle = glm::acos(glm::dot(toEnemy, fighterController->look));
		Game::Instance()->PrintFloat("Angle:", angle);
		if (angle < fov)
		{
			if (timeShot >= 0.25f)
			{
				shared_ptr<LazerBeam> lazer = make_shared<LazerBeam>();
				lazer->position = fighterController->position;
				lazer->look = fighterController->look;
				Game::Instance()->Attach(lazer);
				timeShot = 0.0f;
			}
		}
	}
}

string AttackingState::Description()
{
	return "Attacking";
}
