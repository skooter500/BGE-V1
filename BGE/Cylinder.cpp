#include "Cylinder.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

Cylinder::Cylinder(float radius, float height):GameComponent(true)
{
	shared_ptr<Model> model = Content::LoadModel("cyl");
	model->drawMode = Model::draw_modes::single_material;
	Attach(model);
	model->Initialise();
	transform->diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	//specular = glm::vec3(0,0,0);
	tag = "Cylinder";
	
	transform->scale = glm::vec3(radius, height, radius);
}


Cylinder::~Cylinder(void)
{
}
