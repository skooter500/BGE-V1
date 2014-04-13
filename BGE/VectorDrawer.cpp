#include "VectorDrawer.h"
#include "LineDrawer.h"

using namespace BGE;
using namespace std;



VectorDrawer::VectorDrawer(glm::vec3 scale)
{
	float back = 0.2f;
	float side = 0.1f;
	myScale = scale;
	// The lines
	lines.push_back(glm::vec3(0));
	lines.push_back(Transform::basisLook);
	
	lines.push_back(glm::vec3(0));
	lines.push_back(Transform::basisRight);
	
	lines.push_back(glm::vec3(0));
	lines.push_back(Transform::basisUp);
	
	colours.push_back(glm::vec3(1,1,1));
	colours.push_back(glm::vec3(1,0,0));
	colours.push_back(glm::vec3(0,1,0));
}


VectorDrawer::~VectorDrawer(void)
{
}

bool VectorDrawer::Initialise()
{
	return GameComponent::Initialise();
}

void VectorDrawer::Draw()
{
	int step = 2;
	vector<glm::quat> arrowOrientations;

	arrowOrientations.push_back(transform->orientation); // the look
	arrowOrientations.push_back(glm::angleAxis(-90.0f, transform->up) * transform->orientation); // the right
	arrowOrientations.push_back(glm::angleAxis(90.0f, transform->right) * transform->orientation); // the up

	for (int i = 0 ; i < lines.size() ; i += 2)
	{
		glm::mat4 trans = glm::translate(glm::mat4(1), transform->position) * glm::mat4_cast(transform->orientation) * glm::scale(glm::mat4(1), myScale);
		glm::vec4 point0, point1;
		point0 = trans * glm::vec4(lines[i], 1);
		point1 = trans * glm::vec4(lines[i + 1], 1);

		LineDrawer::DrawArrowLine(glm::vec3(point0), glm::vec3(point1), colours[i / step], arrowOrientations[i / step]);
	}
}


