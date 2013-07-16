#include "Box.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

Box::Box():GameComponent()
{
	Model * model = Content::LoadModel("cube");
	model->drawMode = Model::draw_modes::single_material;
	model->diffuse = glm::vec3(1,1,1);
	model->specular = glm::vec3(1,1,1);
	AddChild(model);
}

void Box::Update(float timeDelta)
{
	Yaw(timeDelta * 100.0f);
	GameComponent::Update(timeDelta);
}


Box::~Box(void)
{
}
