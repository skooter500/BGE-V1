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
	model = Content::LoadModel("transporter");
	
	model->Initialise();
	children.push_back(model);
	return true;
}

void ModelTest::Cleanup()
{
	delete model;
}

