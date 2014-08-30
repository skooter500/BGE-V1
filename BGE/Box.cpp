#include "Box.h"
#include "Model.h"
#include "Content.h"
#include "VectorDrawer.h"

using namespace BGE;

Box::Box(float width, float height, float depth):GameComponent(true)
{	
	transform->diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	transform->specular = glm::vec3(0,0,0);
	transform->ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	tag = "Box";
	transform->scale = glm::vec3(width, height, depth);
}

bool Box::Initialise()
{
	shared_ptr<Model> model = Content::LoadModel("cube");
	model->drawMode = Model::draw_modes::single_material;
	Attach(model);
	return model->Initialise();
	
}

void Box::Update(float timeDelta)
{
	GameComponent::Update(timeDelta);
}

Box::~Box(void)
{
}
