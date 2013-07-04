#include "ModelTest.h"
#include "Content.h"
#include "Model.h"

using namespace BGE;

ModelTest::ModelTest(void) {
}


ModelTest::~ModelTest(void) {
}

bool ModelTest::Initialise() {

	// Initialise OpenGL, GLEW and SDL
	if (!Game::Initialise()) {
		return false;
	}
	model = Content::LoadModel("cobramk3");
	//model->position = glm::vec3(0, 0, -20);
	model->Initialise();
	children.push_back(model);
	return true;
}

void ModelTest::Cleanup()
{
	delete model;
}

