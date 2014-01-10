#include "CapsuleShape.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

CapsuleShape::CapsuleShape(float radius, float height)
{
	shared_ptr<Model> model = Content::LoadModel("capsule");
	model->drawMode = Model::draw_modes::single_material;
	model->Initialise();
	diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	//specular = glm::vec3(0,0,0);
	tag = "Capsule";
	Attach(model);
	//scale = glm::vec3(height, radius, radius);
	scale = glm::vec3(radius, height, radius);
}

CapsuleShape::~CapsuleShape(void)
{

}
