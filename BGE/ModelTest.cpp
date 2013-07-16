#include "ModelTest.h"
#include "Content.h"
#include "Model.h"
#include "Ground.h"

using namespace BGE;

ModelTest::ModelTest(void) {
}


ModelTest::~ModelTest(void) {
}

bool ModelTest::Initialise() {
	model = Content::LoadModel("cobramk3");
	model->position = glm::vec3(0, 0, -20);
	children.push_back(model);
	//Ground * ground = new Ground();
	//children.push_back(ground);	
	// Initialise OpenGL, GLEW and SDL
	if (!Game::Initialise()) {
		return false;
	}
	return true;
}

void ModelTest::Cleanup()
{
	delete model;
}

