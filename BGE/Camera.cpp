#include "Camera.h"
#include "Game.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace BGE;
using namespace std;
#include <SDL.h>
#include "Params.h"

Camera::Camera(void):GameComponent(true)
{
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000000.0f);		
}

Camera::~Camera(void)
{
}

bool Camera::Initialise()
{
	startPos = Params::GetVector3("camPos");
	transform->position  = startPos;
	SDL_WarpMouseInWindow(
		Game::Instance()->GetMainWindow()
		,Params::GetFloat("width") / 2
		, Params::GetFloat("height") / 2);
	SDL_ShowCursor(SDL_DISABLE); 
	return GameComponent::Initialise();
}

void Camera::Draw()
{
	//GameComponent::Draw();
}

void Camera::Update(float timeDelta) {
	// Update my children first
	GameComponent::Update(timeDelta);
	view = glm::lookAt(
		transform->position
		, transform->position + transform->look
		, Transform::basisUp
		);
	stringstream ss;
	Game::Instance()->PrintVector("Camera Pos: ", transform->position);
	Game::Instance()->PrintVector("Camera Look: ", transform->look);
	Game::Instance()->PrintVector("Camera Up: ", transform->up);
	Game::Instance()->PrintVector("Camera Right: ", transform->right);	
}