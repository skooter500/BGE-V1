#include "ModelTest.h"
#include "Content.h"
#include "Model.h"
#include "Ground.h"
#include "Box.h"
#include "Sphere.h"
#include "Steerable3DController.h"
#include "Cylinder.h"

using namespace BGE;

ModelTest::ModelTest(void) {
}


ModelTest::~ModelTest(void) {
}

bool ModelTest::Initialise() {
	
	std::shared_ptr<GameComponent> ground(new Ground());
	children.push_back(ground);	

	std::shared_ptr<GameComponent> box (new Box(1, 1, 1));
	box->position = glm::vec3(0, 5, -20);
	AddChild(box);

	/*std::shared_ptr<GameComponent> box1 (new Box(1, 2, 1));
	box1->position = glm::vec3(5, 2, 0);
	box->AddChild(box1);

	std::shared_ptr<GameComponent> sphere (new Sphere(1));
	sphere->position = glm::vec3(10, 5, -20);
	children.push_back(sphere);
	
	std::shared_ptr<GameComponent> cyl (new Cylinder(2, 1));
	cyl->position = glm::vec3(15, 5, -20);
	children.push_back(cyl);
	
	std::shared_ptr<GameComponent> ship = make_shared<GameComponent>();
	std::shared_ptr<Model> model = Content::LoadModel("marimba");	
	std::shared_ptr<GameComponent> steerable = make_shared<Steerable3DController>(model);
	steerable->position = glm::vec3(20, 5, -20);
	ship->AddChild(steerable);
	ship->AddChild(model);
	AddChild(ship);*/
	
	//Initialise OpenGL, GLEW and SDL and then initialize all my children!
	if (!Game::Initialise()) {
		return false;
	}
	return true;
}