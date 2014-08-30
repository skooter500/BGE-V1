#include "SceneGraphGame.h"

using namespace BGE;

shared_ptr<GameComponent> ship4;

SceneGraphGame::SceneGraphGame(void) :Game()
{
}

SceneGraphGame::~SceneGraphGame(void)
{
}

glm::vec3 SceneGraphGame::NextPosition(float step, float steps)
{
	float radius = 30.0f;
	float y = 5.0f;
	float inc = (glm::pi<float>() * 2.0f) / steps;
	float angle = step * inc;
	glm::vec3 pos;
	
	pos.x = glm::sin(angle) * radius;
	pos.y = y;
	pos.z = - glm::cos(angle) * radius;
	waypoints.push_back(pos);
	return pos;
}

bool SceneGraphGame::Initialise()
{
	Params::Load("default");
	
	dynamicsWorld->setGravity(btVector3(0,0,0));
	
	camera->transform->position = glm::vec3(-10,20,20);
	camera->transform->look = glm::vec3(0, 0, 1); 

	physicsFactory->CreateCameraPhysics();
	physicsFactory->CreateGroundPhysics();

	fullscreen = true;
	riftEnabled = true;
	console = false;
	width = 1280;
	height = 800;

	CreateScene();

	
	
	return Game::Initialise();
}

void SceneGraphGame::Update(float timeDelta)
{
	dynamicsWorld->stepSimulation(timeDelta,100);

	selfExample->transform->Yaw(timeDelta * speed * speed);	
	station->transform->Yaw(timeDelta * speed * speed);
	Game::Update(timeDelta);
}

void SceneGraphGame::CreateScene()
{
	float componentCount = 10.0f;
	float current = 0.0f;
	shared_ptr<GameComponent> partFollower = make_shared<GameComponent>(true);
	Attach(partFollower);

	selfExample = make_shared<GameComponent>(true);
	selfExample->Attach(Content::LoadModel("ferdelance", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0, 1, 0))));
	selfExample->transform->specular = glm::vec3(1.2f, 1.2f, 1.2f);

	selfExample->Attach(make_shared<VectorDrawer>(glm::vec3(5, 5, 5)));
	selfExample->transform->position = NextPosition(current++, componentCount);
	Attach(selfExample);

	station = make_shared<GameComponent>(true);
	station->transform->ambient = glm::vec3(0.2f, 0.2, 0.2f);
	station->transform->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	station->transform->scale = glm::vec3(1, 1, 1);
	std::shared_ptr<Model> cmodel = Content::LoadModel("coriolis", glm::rotate(glm::mat4(1), 90.0f, Transform::basisUp));
	station->Attach(cmodel);
	station->Attach(make_shared<VectorDrawer>(glm::vec3(5, 5, 5)));
	station->transform->position = NextPosition(current++, componentCount);
	Attach(station);

	// Add a child to the station and update by including the parent's world transform
	ship1 = make_shared<GameComponent>(true);
	ship1->transform->ambient = glm::vec3(0.2f, 0.2, 0.2f);
	ship1->transform->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	ship1->tag = "blah";
	std::shared_ptr<Model> ana = Content::LoadModel("anaconda", glm::rotate(glm::mat4(1), 180.0f, Transform::basisUp));
	ship1->Attach(ana);
	ship1->transform->position = glm::vec3(0, 0, -10);
	station->Attach(ship1); // NOTE the ship is attached to the station at an offset of 10.

	// Create a component  with an XBOX Controller attached
	shared_ptr<GameComponent> ship2 = make_shared<GameComponent>(true);
	ship2->Attach(make_shared<XBoxController>());
	ship2->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0, 1, 0))));
	ship2->Attach(make_shared<VectorDrawer>(glm::vec3(5, 5, 5)));
	ship2->transform->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	ship2->transform->position = NextPosition(current++, componentCount);
	Attach(ship2);

	// Create a component with a steerable 3D controller attached
	shared_ptr<GameComponent> ship3 = make_shared<GameComponent>(true);
	ship3->transform->scale = glm::vec3(3, 3, 3);
	shared_ptr<Model> s3Model = Content::LoadModel("moray", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0, 1, 0)));
	ship3->Attach(make_shared<Steerable3DController>(s3Model));
	ship3->Attach(s3Model);
	ship3->transform->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	ship3->Attach(make_shared<VectorDrawer>(glm::vec3(5, 5, 5)));
	ship3->transform->position = NextPosition(current++, componentCount);
	Attach(ship3);

	// Create some physics components using the factory
	physicsFactory->CreateBox(5, 5, 5, NextPosition(current++, componentCount), glm::quat());
	physicsFactory->CreateFromModel("monkey", NextPosition(current++, componentCount), glm::quat(), glm::vec3(5, 5, 5));

	//// Create a physics car
	physicsFactory->CreateVehicle(NextPosition(current++, componentCount));
	//
	// Create a physics component and attach a non-physics component to it
	shared_ptr<PhysicsController> carController = physicsFactory->CreateVehicle(NextPosition(current++, componentCount));

	shared_ptr<GameComponent> transporter = make_shared<GameComponent>(true);
	transporter->transform->position = glm::vec3(0, 5, 0);
	transporter->Attach(Content::LoadModel("transporter"));
	carController->parent->Attach(transporter);

	//// Create some steering components to chase each other
	ship4 = make_shared<GameComponent>(true);
	// Create some steering components to chase each other
	ship4->tag = "Steerable";
	ship4->transform->scale = glm::vec3(2, 2, 2);
	ship4->Attach(Content::LoadModel("krait", glm::rotate(glm::mat4(1), 180.0f, Transform::basisUp)));
	ship4->transform->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	shared_ptr<SteeringController> ship4Controller = make_shared<SteeringController>();

	ship4->Attach(ship4Controller);
	ship4Controller->transform->position = NextPosition(current++, componentCount);
	ship4Controller->TurnOffAll();
	ship4Controller->TurnOn(SteeringController::behaviour_type::follow_path);
	ship4Controller->route->waypoints.push_back(ship4Controller->transform->position);
	ship4Controller->route->waypoints.push_back(ship4Controller->transform->position + glm::vec3(15, 0, 0));
	ship4Controller->route->waypoints.push_back(ship4Controller->transform->position + glm::vec3(15, 0, -15));
	ship4Controller->route->waypoints.push_back(ship4Controller->transform->position + glm::vec3(0, 0, -15));
	ship4Controller->route->looped = true;
	ship4Controller->route->draw = true;

	Attach(ship4);

	// Load a textured model
	shared_ptr<GameComponent> mushroom = make_shared<GameComponent>(true);
	mushroom->transform->position = NextPosition(current++, componentCount);
	mushroom->transform->scale = glm::vec3(0.1f, 0.1f, 0.1f);
	mushroom->Attach(Content::LoadModel("mushroom"));
	Attach(mushroom);

	// Create a component that follows a path from component to component
	partFollower->tag = "Steerable";
	shared_ptr<SteeringController> pathFollowerController = make_shared<SteeringController>();
	partFollower->Attach(pathFollowerController);
	partFollower->transform->position = NextPosition(current, componentCount);
	pathFollowerController->TurnOffAll();
	pathFollowerController->TurnOn(SteeringController::behaviour_type::follow_path);
	pathFollowerController->route->transform->diffuse = glm::vec3(0, 0, 1);
	pathFollowerController->route->draw = true;

	// Add some waypoints
	for (int i = 0; i < waypoints.size(); i++)
	{
		pathFollowerController->route->waypoints.push_back(waypoints[i]);
		pathFollowerController->route->looped = true;
	}

	shared_ptr<FountainEffect> fountain = make_shared<FountainEffect>(1000);
	partFollower->Attach(fountain);
	partFollower->transform->diffuse = glm::vec3(0, 1, 1);
}


