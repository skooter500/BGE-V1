#include "VectorDrawer.h"
#include "LineDrawer.h"

using namespace BGE;
using namespace std;



VectorDrawer::VectorDrawer(glm::vec3 scale)
{
	float back = 0.2f;
	float side = 0.1f;
	this->scale = scale;

	// The lines
	lines.push_back(glm::vec3(0));
	lines.push_back(GameComponent::basisLook);
	
	lines.push_back(glm::vec3(0));
	lines.push_back(GameComponent::basisRight);
	
	lines.push_back(glm::vec3(0));
	lines.push_back(GameComponent::basisUp);
	
	colours.push_back(glm::vec3(1,1,1));
	colours.push_back(glm::vec3(1,0,0));
	colours.push_back(glm::vec3(0,1,0));

	worldMode = GameComponent::from_parent;
}


VectorDrawer::~VectorDrawer(void)
{
}

bool VectorDrawer::Initialise()
{
	myScale = scale;
	return GameComponent::Initialise();
}

void VectorDrawer::Draw()
{
	int step = 2;
	vector<glm::quat> arrowOrientations;

	arrowOrientations.push_back(orientation); // the look
	arrowOrientations.push_back(glm::angleAxis(-90.0f, up) * orientation); // the right
	arrowOrientations.push_back(glm::angleAxis(90.0f, right) * orientation); // the up

	for (int i = 0 ; i < lines.size() ; i += 2)
	{
		glm::mat4 trans = glm::translate(glm::mat4(1), position) * glm::mat4_cast(orientation) * glm::scale(glm::mat4(1), myScale);
		glm::vec4 point0, point1;
		point0 = trans * glm::vec4(lines[i], 1);
		point1 = trans * glm::vec4(lines[i + 1], 1);

		LineDrawer::DrawArrowLine(glm::vec3(point0), glm::vec3(point1), colours[i / step], arrowOrientations[i / step]);
	}
}
