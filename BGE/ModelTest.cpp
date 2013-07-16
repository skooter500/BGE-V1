#include "ModelTest.h"
#include "Content.h"
#include "Model.h"
#include "Ground.h"
#include "Box.h"

using namespace BGE;

ModelTest::ModelTest(void) {
}


ModelTest::~ModelTest(void) {
}

bool ModelTest::Initialise() {
	
	Ground * ground = new Ground();
	children.push_back(ground);	
	Box * box = new Box();
	box->position = glm::vec3(0, 5, -20);
	children.push_back(box);
	
	//Initialise OpenGL, GLEW and SDL
	if (!Game::Initialise()) {
		return false;
	}
	return true;
}

void ModelTest::Cleanup()
{
	//delete model;
}

