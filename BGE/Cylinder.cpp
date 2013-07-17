#include "Cylinder.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

Cylinder::Cylinder(float height, float radius)
{
	Model * model = Content::LoadModel("cyl");
	drawMode = Model::draw_modes::single_material;
	diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	specular = glm::vec3(0,0,0);
	AddChild(model);
	scale = glm::vec3(radius, height, radius);
}


Cylinder::~Cylinder(void)
{
}
