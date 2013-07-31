#include "ModelTest.h"
#include "Content.h"
#include "Model.h"
#include "Ground.h"
#include "Box.h"
#include "Sphere.h"
#include "Ship.h"
#include "Cylinder.h"

using namespace BGE;

ModelTest::ModelTest(void) {
}


ModelTest::~ModelTest(void) {
}

bool ModelTest::Initialise() {
	
	Ground * ground = new Ground();
	children.push_back(ground);	

	Box * box = new Box(1, 1, 1);
	box->position = glm::vec3(0, 5, -20);
	AddChild(box);

	Box * box1 = new Box(1, 2, 1);
	box1->position = glm::vec3(5, 2, 0);
	box->AddChild(box1);

	Sphere * sphere = new Sphere(1);
	sphere->position = glm::vec3(10, 5, -20);
	children.push_back(sphere);
	
	Cylinder * cyl = new Cylinder(2, 1);
	cyl->position = glm::vec3(15, 5, -20);
	children.push_back(cyl);
	
	Ship * ship = new Ship();
	ship->position = glm::vec3(20, 5, -20);
	children.push_back(ship);

	//Initialise OpenGL, GLEW and SDL and then initialize all my children!
	if (!Game::Initialise()) {
		return false;
	}
	return true;
}