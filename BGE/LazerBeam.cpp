#include "LazerBeam.h"
#include "LineDrawer.h"

using namespace BGE;

LazerBeam::LazerBeam(void) :GameComponent(true)
{
}


LazerBeam::~LazerBeam(void)
{
}

void LazerBeam::Update( float timeDelta )
{
	float width = 500;
	float height = 500;
	float speed = 5.0f;
	if ((transform->position.x < - (width / 2)) || (transform->position.x > width / 2) || (transform->position.z < - (height / 2)) || (transform->position.z > height / 2) || (transform->position.y < 0) || (transform->position.y > 100))
	{
		alive = false;
	}
	transform->position += transform->look * speed;
}

void BGE::LazerBeam::Draw()
{
	LineDrawer::DrawLine(transform->position, transform->position + transform->look * 10.0f, glm::vec3(1,0,0));
}


