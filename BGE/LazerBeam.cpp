#include "LazerBeam.h"
#include "LineDrawer.h"

using namespace BGE;

LazerBeam::LazerBeam(void)
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
	if ((position.x < - (width / 2)) || (position.x > width / 2) || (position.z < - (height / 2)) || (position.z > height / 2) || (position.y < 0) || (position.y > 100))
	{
		alive = false;
	}
	position += look * speed;
}

void BGE::LazerBeam::Draw()
{
	LineDrawer::DrawLine(position, position + look * 10.0f, glm::vec3(1,0,0));
}


