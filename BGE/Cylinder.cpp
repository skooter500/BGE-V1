#include "Cylinder.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

Cylinder::Cylinder(float radius, float height)
{
	shared_ptr<Model> model = Content::LoadModel("cyl");
	model->drawMode = Model::draw_modes::single_material;
	model->Initialise();
	diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	//specular = glm::vec3(0,0,0);
	tag = "Cylinder";
	Attach(model);
	scale = glm::vec3(radius, height, radius);
}


Cylinder::~Cylinder(void)
{
}
