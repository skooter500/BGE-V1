#include "Camera.h"
#include "Game.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace BGE;
using namespace std;
#include <SDL.h>



Camera::Camera(void):GameComponent()
{
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);		
	position = glm::vec3(0.0f, 10.0f, 10.0f);

	
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
	SDL_WarpMouseInWindow(
		Game::Instance()->GetMainWindow()
		,Game::Instance()->GetWidth() / 2
		,Game::Instance()->GetHeight() / 2);
	SDL_ShowCursor(SDL_DISABLE); 
	return GameComponent::Initialise();
}

float fps = 0;;

void Camera::Draw()
{
	stringstream msg;
	msg << "FPS: " << (int) fps;
	Game::Instance()->PrintText(msg.str());
	GameComponent::Draw();
}

void Camera::Update(float timeDelta) {
	
	fps = 1.0f / timeDelta;
	const Uint8 * keyState = Game::Instance()->GetKeyState();

	float moveSpeed = speed;

	if (keyState[SDL_SCANCODE_LSHIFT])
	{
		moveSpeed *= 10.0f;
	}
	

	if (keyState[SDL_SCANCODE_W])
	{
		Walk(moveSpeed * timeDelta);
	}

	if (keyState[SDL_SCANCODE_S])
	{
		Walk(-moveSpeed * timeDelta);
	}

	if (keyState[SDL_SCANCODE_A])
	{
		Strafe(-moveSpeed * timeDelta);
	}

	if (keyState[SDL_SCANCODE_D])
	{
		Strafe(moveSpeed * timeDelta);
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
	SDL_WarpMouseInWindow(
		Game::Instance()->GetMainWindow()
		,midX
		,midY
		);
	
	moved = true;
	if (moved) {
		// Camera matrix
		view = glm::lookAt(
			position
			, position + look
			, basisUp
		);
		//cout << position.x << position.y << position.z << endl;
	}
	
	GameComponent::Update(timeDelta);
}
