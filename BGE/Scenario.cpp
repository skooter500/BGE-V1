#include "Scenario.h"
#include "Params.h"
#include "SteeringGame.h"
#include "Content.h"
#include "Utils.h"
#include "Sphere.h"

using namespace BGE;

void Scenario::Setup()
{
	SteeringGame * game = (SteeringGame *) Game::Instance();
	Params::Load("flocking");
	float range = Params::GetFloat("world_range");

	shared_ptr<GameComponent> bigFighter = make_shared<GameComponent>();
	bigFighter->tag = "Steerable";
	glm::vec3 pos = RandomPosition(range);
	shared_ptr<SteeringControler> bigFighterController = make_shared<SteeringControler>();
	bigFighterController->position = bigFighter->position = pos;
	bigFighterController->TurnOn(SteeringControler::behaviour_type::obstacle_avoidance);
	bigFighterController->TurnOn(SteeringControler::behaviour_type::wander);
	bigFighterController->TurnOn(SteeringControler::behaviour_type::pursuit);
	bigFighterController->TurnOn(SteeringControler::behaviour_type::sphere_constrain);
	bigFighter->scale = glm::vec3(5, 5, 5);
	bigFighter->Attach(bigFighterController);
	bigFighter->Attach(Content::LoadModel("python", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp)));
	game->Attach(bigFighter);

	shared_ptr<GameComponent> fighter;
	shared_ptr<SteeringControler> fighterController;
	for (int i = 0; i < Params::GetFloat("num_boids"); i++)
	{
		glm::vec3 pos = RandomPosition(range);

		fighter = make_shared<GameComponent>();
		fighter->tag = "Steerable";
		fighterController = make_shared<SteeringControler>();
		fighterController->position = fighter->position = pos;
		fighterController->target = bigFighter;
		fighterController->TurnOffAll();
		fighterController->TurnOn(SteeringControler::behaviour_type::separation);
		fighterController->TurnOn(SteeringControler::behaviour_type::cohesion);
		fighterController->TurnOn(SteeringControler::behaviour_type::alignment);
		fighterController->TurnOn(SteeringControler::behaviour_type::wander);
		fighterController->TurnOn(SteeringControler::behaviour_type::evade);
		fighterController->TurnOn(SteeringControler::behaviour_type::sphere_constrain);
		fighterController->TurnOn(SteeringControler::behaviour_type::obstacle_avoidance);
		fighter->Attach(Content::LoadModel("ferdelance", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp)));
		fighter->scale = glm::vec3(5,5, 5);
		fighter->Attach(fighterController);
		game->Attach(fighter);
	}

	int numObstacles = 10;
	float dist = (range * 2) / numObstacles;
	for (float x = -range; x < range; x += dist)
	{
		for (float z = -range; z < range; z += dist)
		{
			shared_ptr<Sphere> obstacle = make_shared<Sphere>(10);
			obstacle->tag = "Obstacle";
			obstacle->position = glm::vec3(x, 0, z);
			game->Attach(obstacle);
		}
	}

	bigFighterController->target = fighter;

	
	game->camFollower = make_shared<GameComponent>();
	shared_ptr<SteeringControler> camController = make_shared<SteeringControler>();
	camController->offset = glm::vec3(0,0,3);
	camController->leader = fighter;
	camController->position = game->camFollower->position = fighter->position + camController->offset;
	//fighter->scale = fighterController->scale  = glm::vec3(1002, 1002, 1002);

	camController->TurnOffAll();
	camController->TurnOn(SteeringControler::behaviour_type::offset_pursuit);
	camController->TurnOn(SteeringControler::behaviour_type::sphere_constrain);
	game->Attach(game->camFollower);
	game->camFollower->Attach(camController);
	
}
