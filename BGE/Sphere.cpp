#include "Sphere.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

Sphere::Sphere(float radius):GameComponent(true)
{
	std::shared_ptr<GameComponent> model (Content::LoadModel("sphere"));
	model->drawMode = Model::draw_modes::single_material;
	Attach(model);
	model->Initialise();
	transform->diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	transform->specular = glm::vec3(1.2f,1.2f, 1.2f);
	tag = "Sphere";
	transform->scale = glm::vec3(radius, radius, radius);
}


Sphere::~Sphere(void)
{
}
