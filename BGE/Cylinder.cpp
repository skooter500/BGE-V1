#include "Cylinder.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

Cylinder::Cylinder(float radius, float height):GameComponent(true)
{	
	transform->diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	transform->ambient = glm::vec3(0, 0, 0);
	transform->specular = glm::vec3(1, 1, 1);
	transform->ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	tag = "Cylinder";
	
	transform->scale = glm::vec3(radius, height, radius);
}

bool Cylinder::Initialise()
{
	shared_ptr<Model> model = Content::LoadModel("cyl");
	model->drawMode = Model::draw_modes::single_material;
	Attach(model);
	return model->Initialise();
}

void Cylinder::Draw()
{
	//Game::Instance()->PrintVector("Cyl: ", transform->position);
	GameComponent::Draw();
}

Cylinder::~Cylinder(void)
{
}
