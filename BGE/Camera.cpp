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
	worldMode = world_modes::from_child;
}

Camera::~Camera(void)
{
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
	fps = (int) 1.0f / timeDelta;
	GameComponent::Update(timeDelta);
	moved = true;
	if (moved) {
		// Camera matrix
		view = glm::lookAt(
			position
			, position + look
			, basisUp
			);
	}
	stringstream ss;
	ss << "Camera Pos: " << position.x << " " << position.y << " " << position.z;
	Game::Instance()->PrintText(ss.str());

	ss.str("");
	ss << "Camera Look: " << look.x << " " << look.y << " " << look.z;
	Game::Instance()->PrintText(ss.str());

}