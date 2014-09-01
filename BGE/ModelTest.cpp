#include "ModelTest.h"
#include "Content.h"
#include "Model.h"
#include "Ground.h"
#include "Box.h"
#include "Sphere.h"
#include "Steerable3DController.h"
#include "Cylinder.h"

using namespace BGE;

ModelTest::ModelTest(void) 
{
}

ModelTest::~ModelTest(void) {
}

shared_ptr<GameComponent> rotTest;
float theta = 0.0f;

bool ModelTest::Initialise() 
{
	
	std::shared_ptr<GameComponent> ground = make_shared<Ground>();
	Attach(ground);	

	std::shared_ptr<GameComponent> box = make_shared<Box>(1, 1, 1);
	box->transform->position = glm::vec3(0, 5, -20);
	Attach(box);

	std::shared_ptr<GameComponent> cyl = make_shared<Cylinder>(2, 1);
	cyl->transform->position = glm::vec3(15, 5, -20);
	Attach(cyl);

	std::shared_ptr<GameComponent> box1 = make_shared<Box>(1, 2, 1);
	box1->transform->position = glm::vec3(5, 2, 0);
	box->Attach(box1);

	std::shared_ptr<GameComponent> sphere = make_shared<Sphere>(1);
	sphere->transform->position = glm::vec3(10, 5, -20);
	Attach(sphere);

	rotTest = make_shared<GameComponent>(true);
	rotTest->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	Attach(rotTest);
	
	shared_ptr<GameComponent> ship = make_shared<GameComponent>(true);
	shared_ptr<Model> model = Content::LoadModel("marimba");	
	shared_ptr<GameComponent> steerable = make_shared<Steerable3DController>(model);
	ship->transform->position = glm::vec3(20, 5, -20);
	ship->Attach(steerable);
	ship->Attach(model);
	Attach(ship);

	//Initialise OpenGL, GLEW and SDL and then initialize all my children!
	if (!Game::Initialise()) {
		return false;
	}
	return true;
}

void ModelTest::Update( float timeDelta )
{
	Game::Update(timeDelta);
	glm::mat4 rotMatrix = glm::rotate(glm::mat4(1), theta, glm::vec3(0,1,0));
	rotTest->transform->world = rotMatrix;
	theta += timeDelta * 10.0f;
}
