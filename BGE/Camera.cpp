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
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000000.0f);		

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



void Camera::Draw()
{
	GameComponent::Draw();
}

void Camera::Update(float timeDelta) {
	
	GameComponent::Update(timeDelta);
	view = glm::lookAt(
		transform->position
		, transform->position + transform->look
		, Transform::basisUp
		);
	stringstream ss;
	ss << "Camera Pos: " << transform->position.x << " " << transform->position.y << " " << transform->position.z;
	Game::Instance()->PrintText(ss.str());

	ss.str("");
	ss << "Camera Look: " << transform->look.x << " " << transform->look.y << " " << transform->look.z;
	Game::Instance()->PrintText(ss.str());

	ss.str("");
	ss << "Camera Up: " << transform->up.x << " " << transform->up.y << " " << transform->up.z;
	Game::Instance()->PrintText(ss.str());

	ss.str("");
	ss << "Camera Right: " << transform->right.x << " " << transform->right.y << " " << transform->right.z;
	Game::Instance()->PrintText(ss.str());

}