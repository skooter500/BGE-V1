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
	cameraType = camera_types::free;
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
	camPosFromObj = glm::vec3(0);
	centerAt = glm::vec3(0);
	return GameComponent::Initialise();
}

void Camera::Draw()
{
	GameComponent::Draw();
}


glm::vec3 Camera::GetDistanceWithAngle()
{
	glm::vec3 result = glm::vec3(0);
	
	result.x = pCamVars.fromObj.x * glm::cos(pCamVars.x_angle);
	result.y = pCamVars.fromObj.y;
	result.z = pCamVars.fromObj.z * glm::sin(pCamVars.z_angle);

	return result;
}
void Camera::RearView(shared_ptr<GameComponent> followObject, float distance, float height, float look_distance, float interp)
{
	this->followObject = followObject;
	this->position = followObject->position;
	rCamVars.distance = distance;
	rCamVars.height = height;
	rCamVars.look_distance = look_distance;
	rCamVars.interp = glm::clamp(interp, 0.0f, 0.99f);
	cameraType = camera_types::follow_behind;
}
void Camera::PerspectiveView(shared_ptr<GameComponent> followObject, glm::vec3 distanceFromObject, float x_angle, float z_angle, float look_distance, float interp)
{
	this->followObject = followObject;
	this->position = followObject->position;
	pCamVars.fromObj = distanceFromObject;
	pCamVars.x_angle = x_angle;
	pCamVars.z_angle = z_angle;
	pCamVars.look_distance = look_distance;
	pCamVars.interp = glm::clamp(interp, 0.0f, 0.99f);
	cameraType = camera_types::follow_perspective;
}
void Camera::UnFollow()
{
	if(followObject)
	{
		position = followObject->position;
		followObject = nullptr;
	}
	cameraType = camera_types::free;
}

void Camera::Update(float timeDelta) {
	
	GameComponent::Update(timeDelta);

	if(!followObject) //If follow object doesn't exist revert to free camera
	{
		cameraType = camera_types::free;
	}

	float interp = 0.0f;
	glm::vec3 fakeCamPos = glm::vec3(0); //This is a fake eye position for interpolation(calculated every update)
	glm::vec3 fakeCenter = glm::vec3(0); //This is a fake lookAt position for interpolation(calculated every update)

	switch(cameraType)
	{
	case camera_types::free:
		centerAt = position + look;
		break;
	case camera_types::follow_behind:
		fakeCamPos = followObject->look * rCamVars.distance;
		fakeCamPos.y = rCamVars.height;
		
		interp = rCamVars.interp;
		if(glm::distance(fakeCamPos, camPosFromObj) > glm::epsilon<float>())
		{
			camPosFromObj = interp * camPosFromObj + (1 - interp) * fakeCamPos;
		}

		position = followObject->position + camPosFromObj;
		
		fakeCenter = followObject->position + (followObject->look * rCamVars.look_distance);
		if(glm::distance(fakeCenter, centerAt) > glm::epsilon<float>())
		{
			centerAt = interp * centerAt + (1 - interp) * fakeCenter;
		}
		break;
	case camera_types::follow_perspective:
		interp = pCamVars.interp;

		fakeCamPos = this->GetDistanceWithAngle();

		if(glm::distance(fakeCamPos, camPosFromObj) > glm::epsilon<float>())
		{
			camPosFromObj = interp * camPosFromObj + (1 - interp) * fakeCamPos;
		}

		position = followObject->position + camPosFromObj;
		
		fakeCenter = followObject->position + (followObject->look * pCamVars.look_distance);
		if(glm::distance(fakeCenter, centerAt) > glm::epsilon<float>())
		{
			centerAt = interp * centerAt + (1 - interp) * fakeCenter;
		}
		break;
	}

	view = glm::lookAt(
		position
		, centerAt + look
		, basisUp
		);

	glm::vec3 lookNorm = glm::normalize(centerAt);
	stringstream ss;
	ss << "Camera Pos: " << position.x << " " << position.y << " " << position.z;
	Game::Instance()->PrintText(ss.str());

	ss.str("");
	ss << "Camera Look: " << lookNorm.x << " " << lookNorm.y << " " << lookNorm.z;
	Game::Instance()->PrintText(ss.str());

	//ss.str("");
	//ss << "Camera Up: " << up.x << " " << up.y << " " << up.z;
	//Game::Instance()->PrintText(ss.str());

	//ss.str("");
	//ss << "Camera Right: " << right.x << " " << right.y << " " << right.z;
	//Game::Instance()->PrintText(ss.str());

}