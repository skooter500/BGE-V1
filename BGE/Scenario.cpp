#include "Scenario.h"
#include "Params.h"
#include "SteeringGame.h"
#include "Content.h"
#include "Utils.h"

using namespace BGE;

void Scenario::Setup()
{
	SteeringGame * game = (SteeringGame *) Game::Instance();

	Params::Load("flocking");
	
	/*shared_ptr<GameComponent> bigFighter = make_shared<GameComponent>();
	bigFighter->id = "Steerable";
	shared_ptr<SteeringControler> bigFighterController = make_shared<SteeringControler>();
	bigFighterController->TurnOn(SteeringControler::behaviour_type::obstacle_avoidance);
	bigFighterController->TurnOn(SteeringControler::behaviour_type::wander);
	bigFighterController->TurnOn(SteeringControler::behaviour_type::pursuit);
	bigFighterController->TurnOn(SteeringControler::behaviour_type::sphere_constrain);
	bigFighter->scale = glm::vec3(10,10,10);
	bigFighter->Attach(bigFighterController);
	bigFighter->Attach(Content::LoadModel("python", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp)));
	game->Attach(bigFighter);*/

	float range = Params::GetFloat("world_range");

	shared_ptr<GameComponent> fighter;
	for (int i = 0; i < Params::GetFloat("num_boids"); i++)
	{
		glm::vec3 pos = RandomPosition(range);

		fighter = make_shared<GameComponent>();
		fighter->id = "Steerable";
		shared_ptr<SteeringControler> fighterController = make_shared<SteeringControler>();
		fighterController->position = pos;
		//fighterController->target = bigFighter;
		fighterController->TurnOffAll();
		fighterController->TurnOn(SteeringControler::behaviour_type::separation);
		fighterController->TurnOn(SteeringControler::behaviour_type::cohesion);
		fighterController->TurnOn(SteeringControler::behaviour_type::alignment);
		fighterController->TurnOn(SteeringControler::behaviour_type::wander);
		fighterController->TurnOn(SteeringControler::behaviour_type::sphere_constrain);
		fighterController->TurnOn(SteeringControler::behaviour_type::obstacle_avoidance);
		fighter->Attach(Content::LoadModel("ferdelance", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp)));
		fighter->Attach(fighterController);
		game->Attach(fighter);
	}

	/*
	int numObstacles = 10;
	float dist = (range * 2) / numObstacles;
	for (float x = -range; x < range; x += dist)
	{
		for (float z = -range; z < range; z += dist)
		{
			Obstacle o = new Obstacle(20);
			o.Position = new Vector3(x, 0, z);
			o.Color = new Vector3((float)Utilities.RNG.NextDouble(), (float)Utilities.RNG.NextDouble(), (float)Utilities.RNG.NextDouble());
			o.ShouldDraw = true;
			children.Add(o);
		}
	}
	*/
	//bigFighterController->target = fighter;

	/*Fighter camFighter = new EliteFighter();
	Vector3 offset = new Vector3(0, 0, 10);
	fighter.ModelName = "cobramk3";
	camFighter.Position = fighter.Position + offset;
	camFighter.offset = offset;
	camFighter.Leader = fighter;
	camFighter.SteeringBehaviours.turnOffAll();
	camfighterController->TurnOn(SteeringControler::behaviour_type::offset_pursuit);
	fighterController->TurnOn(SteeringControler::behaviour_type::sphere_constrain);
	XNAGame.Instance.Children.Add(camFighter);

	XNAGame.Instance.CamFighter = camFighter;
	Camera camera = XNAGame.Instance.Camera;
	camera.Position = new Vector3(0.0f, 60.0f, 200.0f);

	foreach (Entity child in children)
	{
	child.LoadContent();
	}*/
}
