#include "Box.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

Box::Box(float width, float height, float depth):GameComponent()
{
	
	shared_ptr<Model> model = Content::LoadModel("cube");
	model->drawMode = drawMode = Model::draw_modes::single_material;
	diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	specular = glm::vec3(0,0,0);
	ambient = glm::vec3(0.2f,0.2f,0.2f);
	scale = glm::vec3(width, height, depth);
	id = "Box";
	AddChild(model);
}

void Box::Update(float timeDelta)
{
	GameComponent::Update(timeDelta);
}

Box::~Box(void)
{
}
