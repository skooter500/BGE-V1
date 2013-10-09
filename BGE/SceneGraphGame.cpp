#include "SceneGraphGame.h"
#include "Content.h"

using namespace BGE;

shared_ptr<GameComponent> selfExample;

SceneGraphGame::SceneGraphGame(void)
{
	physicsFactory = NULL;
	dynamicsWorld = NULL;
	broadphase = NULL;
	dispatcher = NULL;
	solver = NULL;
	
}

SceneGraphGame::~SceneGraphGame(void)
{
}

glm::vec3 NextPosition(int step, int steps)
{
	float radius = 10.0f;
	float y = 5.0f;
	float inc = (glm::pi<float>() * 2.0f) / steps;
}

bool SceneGraphGame::Initialise()
{
	// Set up the collision configuration and dispatcher
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The world.
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	broadphase = new btAxisSweep3(worldMin,worldMax);
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-9,0));

	camera->position = glm::vec3(0,0,0);
	camera->look = glm::vec3(0, 0, 1);

	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);

	physicsFactory->CreateCameraPhysics();
	physicsFactory->CreateGroundPhysics();


	selfExample = make_shared<GameComponent>();
	selfExample->Attach(Content::LoadModel("ferdelance", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	selfExample->position = glm::vec3(-20, 0, -20);
	Attach(selfExample);


	fullscreen = false;
	riftEnabled = false;
	width = 800;
	height = 600;

	return Game::Initialise();
}

void SceneGraphGame::Update(float update)
{
	Game::Update(update);
}

void SceneGraphGame::Cleanup()
{
	Game::Cleanup();
}


