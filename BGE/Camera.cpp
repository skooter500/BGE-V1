#include "Camera.h"
#include "Game.h"
#include <iostream>

using namespace BGE;
using namespace std;

Camera::Camera(void):GameComponent()
{
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);		
	position = glm::vec3(0.0f, 0.0f, 10.0f);
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

bool Camera::Initialise()
{
	SDL_WarpMouse(Game::Instance()->GetWidth() / 2,Game::Instance()->GetHeight() / 2);
	SDL_ShowCursor(SDL_DISABLE); 
	return GameComponent::Initialise();
}

void Camera::Update(float timeDelta) {

	Uint8 * keyState = Game::Instance()->GetKeyState();
	if (keyState[SDLK_w])
	{
		Walk(speed * timeDelta);
	}

	if (keyState[SDLK_s])
	{
		Walk(-speed * timeDelta);
	}

	if (keyState[SDLK_a])
	{
		Strafe(-speed * timeDelta);
	}

	if (keyState[SDLK_d])
	{
		Strafe(speed * timeDelta);
	}

	int x, y;
	int midX, midY;
    SDL_GetMouseState(&x,&y);
	midX = Game::Instance()->GetWidth() / 2;
	midY = Game::Instance()->GetHeight() / 2;
	float yaw, pitch;
	yaw = midX - x;
	pitch = midY - y;

	float scale = 0.1f;
	if (yaw != 0)
	{
		Yaw(yaw * scale);
	}
	if (pitch != 0)
	{
		Pitch(pitch * scale);
	}
	SDL_WarpMouse(midX, midY);

	if (moved) {
		// Camera matrix
		view = glm::lookAt(
			position
			, position + look
			, up
		);
		//cout << position.x << position.y << position.z << endl;
	}
	
	GameComponent::Update(timeDelta);
}
