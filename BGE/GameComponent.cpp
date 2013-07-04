#include "GameComponent.h"
#include <iostream>
#include <cmath>
#include <gtx\constants.hpp>
using namespace BGE;
using namespace std;

const glm::vec3 GameComponent::globalUp = glm::vec3(0, 1, 0);
const glm::vec3 GameComponent::basis = glm::vec3(0, 0, -1);

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
	std::list<GameComponent *>::iterator it = children.begin();
	while (it != children.end())
	{
		delete *it;		
	}
}

bool GameComponent::Initialise()
{
	// Initialise all the children
	std::list<GameComponent *>::iterator it = children.begin();
	while (it != children.end())
	{
		if (! (*it ++)->Initialise())
		{
			return false;
		}
	}
	return true;
}

void GameComponent::Draw()
{
	// Draw all the children
	std::list<GameComponent *>::iterator it = children.begin();
	while (it != children.end())
	{
		(*it ++)->Draw();		
	}
}

void GameComponent::Cleanup()
{
	// Cleanup all the children
	std::list<GameComponent *>::iterator it = children.begin();
	while (it != children.end())
	{
		(*it ++)->Cleanup();	
	}
}


void GameComponent::Update(float timeDelta) {
	
	float dot = glm::dot(look, GameComponent::basis);
	float theta;
	theta = acos(dot);
	
	if (_isnan(theta))
	{
		theta = 0.0f;
	}
	if (look.x < 0.0f)
	{
		theta = (glm::pi<float>() * 2.0f) - theta;  
	}

	world = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::degrees(theta), GameComponent::globalUp);
	moved = false;

	// Update all the children
	std::list<GameComponent *>::iterator it = children.begin();
	while (it != children.end())
	{
		(*it ++)->Update(timeDelta);
	}
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

	glm::vec4 tlook = glm::vec4(look, 0);
	tlook = pitch * tlook;
	look = glm::vec3(tlook);

	glm::vec4 tup = glm::vec4(up, 0);
	tlook = pitch * tup;
	up = glm::vec3(tup);
	
	moved = true;
}

void GameComponent::Yaw(float angle)
{
	glm::mat4 yaw;
	yaw = glm::rotate(yaw, angle, up); 

	glm::vec4 tlook = glm::vec4(look, 0);
	tlook = yaw * tlook;
	look = glm::vec3(tlook);

	glm::vec4 tright = glm::vec4(right, 0);
	tright = yaw * tright;
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

void GameComponent::AddChild(GameComponent * child)
{
	children.push_back(child);
}


std::list<GameComponent *> * GameComponent::GetChildren()
{
	return & children;
}