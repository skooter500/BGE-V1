#include "GameComponent.h"
#include <iostream>

using namespace BGE;
using namespace std;

GameComponent::GameComponent(void)
{
	position = glm::vec3(0, 0, 0); 
	look = glm::vec3(0, 0, -1); 
	right = glm::vec3(1, 0, 0); 
	up = glm::vec3(0, 1, 0); 
	velocity = glm::vec3(0, 0, 0); 
	moved = true;
	speed = 10.0f;
}

GameComponent::~GameComponent(void)
{
}

void GameComponent::Update(float timeDelta) {
	world = glm::translate(glm::mat4(1.0f), position);
	moved = false;
}

void GameComponent::Walk(float units)
{
	position += look * units;
	moved = true;
}

void GameComponent::Strafe(float units)
{
	position += right * units;
	moved = true;
}

void GameComponent::Fly(float units)
{
	position.y += units;
	moved = true;
}

void GameComponent::Pitch(float angle)
{
	glm::mat4 pitch;
	pitch = glm::rotate(pitch, angle, right);
	
	
	moved = true;
}

void GameComponent::Yaw(float angle)
{
	glm::mat4 rot;
	rot = glm::rotate(rot, angle, up); 

	glm::vec4 tlook = glm::vec4(look, 0);
	tlook = rot * tlook;
	look = glm::vec3(tlook);

	glm::vec4 tright = glm::vec4(right, 0);
	tright = rot * tright;
	right = glm::vec3(tright);

	moved = true;
}

void GameComponent::Roll(float angle)
{
	/*
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &_look,	angle);

		// rotate _up and _right around _look vector
	D3DXVec3TransformCoord(&_right,&_right, &T);
	D3DXVec3TransformCoord(&_up,&_up, &T);

	_moved = true;
	*/
	moved = true;
}
