#include "Box.h"
#include "Model.h"
#include "Content.h"
#include "VectorDrawer.h"

using namespace BGE;

Box::Box(float width, float height, float depth):GameComponent()
{
	shared_ptr<Model> model = Content::LoadModel("cube");
	model->drawMode = Model::draw_modes::single_material;
	model->Initialise();
	diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	specular = glm::vec3(0,0,0);
	tag = "Box";
	Attach(model);
	scale = glm::vec3(width, height, depth);
}

void Box::Update(float timeDelta)
{
	GameComponent::Update(timeDelta);
}

Box::~Box(void)
{
}
