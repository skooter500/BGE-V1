#include "Box.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

Box::Box(float width, float height, float depth):GameComponent()
{
	
	Model * model = Content::LoadModel("cube");
	model->drawMode = drawMode = Model::draw_modes::single_material;
	diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	specular = glm::vec3(0,0,0);
	ambient = glm::vec3(0.2f,0.2f,0.2f);
	AddChild(model);
	scale = glm::vec3(width, height, depth);
}

void Box::Update(float timeDelta)
{
	//Yaw(timeDelta * 100.0f);
	GameComponent::Update(timeDelta);
}

Box::~Box(void)
{
}
