#include "Box.h"
#include "Model.h"
#include "Content.h"
#include "VectorDrawer.h"

using namespace BGE;

Box::Box(float width, float height, float depth):GameComponent(true)
{
	shared_ptr<Model> model = Content::LoadModel("cube");
	model->drawMode = Model::draw_modes::single_material;
	Attach(model);
	model->Initialise();
	transform->diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	transform->specular = glm::vec3(0,0,0);
	tag = "Box";
	transform->scale = glm::vec3(width, height, depth);
}

void Box::Update(float timeDelta)
{
	GameComponent::Update(timeDelta);
}

Box::~Box(void)
{
}
