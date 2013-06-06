#include "Camera.h"
#include "Game.h"
#include <iostream>

using namespace BGE;
using namespace std;

Camera::Camera(void):GameComponent()
{
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);		
}

Camera::~Camera(void)
{
}

glm::mat4 Camera::GetView()
{
	return view;
}

glm::mat4 Camera::GetProjection()
{
	return projection;
}

void Camera::Update(float timeDelta) {

	Uint8 * keyState = Game::Instance()->GetKeyState();
	if (keyState[SDLK_w])
	{
		Walk(speed * timeDelta);
	}

	if (moved) {
		// Camera matrix
		view = glm::lookAt(
			position
			, position + look
			, up
		);
		cout << position.x << position.y << position.z << endl;
	}
	
	GameComponent::Update(timeDelta);
}
