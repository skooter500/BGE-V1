#include "Sphere.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

Sphere::Sphere(float radius)
{
	Model * model = Content::LoadModel("sphere");
	model->drawMode = drawMode = Model::draw_modes::single_material;
	
	diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	specular = glm::vec3(0.2f, 0.2f, 0.2f);
	AddChild(model);
	scale = glm::vec3(radius, radius, radius);
}


Sphere::~Sphere(void)
{
}
