#include "GameComponent.h"
#include <iostream>
#include <cmath>
#include <gtx\constants.hpp>
using namespace BGE;
using namespace std;

const glm::vec3 GameComponent::basisUp = glm::vec3(0, 1, 0);
const glm::vec3 GameComponent::basisLook = glm::vec3(0, 0, -1);
const glm::vec3 GameComponent::basisRight = glm::vec3(1, 0, 0);

GameComponent::GameComponent(void)
{
	position = glm::vec3(0, 0, 0); 
	look = glm::vec3(0, 0, -1); 
	right = glm::vec3(1, 0, 0); 
	up = glm::vec3(0, 1, 0); 
	velocity = glm::vec3(0, 0, 0); 
	world = glm::mat4(1.0f); // Identity
	orientation = glm::quat(); // Identity
	moved = true;
	speed = 10.0f;
	parent = NULL;
	scale = glm::vec3(1.0, 1.0, 1.0);
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
	world = glm::translate(glm::mat4(1), position) * glm::mat4_cast(orientation) *  glm::scale(glm::mat4(1), scale);
	if (parent != NULL)
	{
		 world = world * parent->world;
	}
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
	// A pitch is a rotation around the right vector
	glm::quat rot = glm::angleAxis(angle, basisRight);

	orientation = rot * orientation;
	look = basisLook * orientation;
	up = basisUp;

	/*
	glm::mat4 pitch;
	pitch = glm::rotate(pitch, angle, right);

	glm::vec4 tlook = glm::vec4(look, 0);
	tlook = pitch * tlook;
	look = glm::vec3(tlook);

	glm::vec4 tup = glm::vec4(up, 0);
	tlook = pitch * tup;
	up = glm::vec3(tup);
	*/
	moved = true;
}

void GameComponent::Yaw(float angle)
{
	// A yaw is a rotation around the global up vector
	glm::quat rot = glm::angleAxis(angle, basisUp);

	orientation = rot * orientation;
	look = basisLook * orientation;
	right = basisRight * orientation;


	/*
	glm::mat4 yaw;
	yaw = glm::rotate(yaw, angle, up); 

	glm::vec4 tlook = glm::vec4(look, 0);
	tlook = yaw * tlook;
	look = glm::vec3(tlook);

	glm::vec4 tright = glm::vec4(right, 0);
	tright = yaw * tright;
	right = glm::vec3(tright);
	*/
	moved = true;
}

void GameComponent::RotateVectors()
{
	look = orientation * look;
	right = orientation * right;
	up = orientation * up;

}

void GameComponent::Roll(float angle)
{
	
	moved = true;
}

void GameComponent::AddChild(GameComponent * child)
{
	child->parent = this;
	children.push_back(child);
}


std::list<GameComponent *> * GameComponent::GetChildren()
{
	return & children;
}