#include "GameComponent.h"

using namespace BGE;

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
	/*
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &_right,	angle);

	// rotate _up and _look around _right vector
	D3DXVec3TransformCoord(&_up,&_up, &T);
	D3DXVec3TransformCoord(&_look,&_look, &T);

	_moved = true;
	*/
	moved = true;
}

void GameComponent::Yaw(float angle)
{
	/*
	D3DXMATRIX T;

	D3DXMatrixRotationY(&T, angle);

	// rotate _right and _look around _up or y-axis
	D3DXVec3TransformCoord(&_right,&_right, &T);
	D3DXVec3TransformCoord(&_look,&_look, &T);

	_moved = true;
	*/
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
