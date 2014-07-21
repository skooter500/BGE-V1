#include "GameComponent.h"
#include "Model.h"
#include "Game.h"
#include <iostream>
#include <cmath>
//#include <glm/gtx/constants.hpp>
#include <ctime>
#include "VectorDrawer.h"

using namespace BGE;
using namespace std;

float BGE::RandomFloat()
{	
	return (float)rand()/(float)RAND_MAX;
}

GameComponent::GameComponent(bool hasTransform)
{
	speed = 10.0f;
	parent = NULL;
	tag = "Nothing";
	if (hasTransform)
	{
		transform = make_shared<Transform>();
	}
	else
	{
		transform = nullptr;
	}
	transformOwner = hasTransform;
	alive = true;
	initialised = false;
	isRelative = false; 
}


bool GameComponent::Initialise()
{	
	// Initialise all the children
	
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	
	while (it != children.end())
	{
		(*it)->initialised = (*it)->Initialise();	
		*it++; 
	}
	
	return true;
}

void GameComponent::Draw()
{
	// Draw all the children
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		// This is necessary for models etc that are instanced
		// As they may be attached to several different parents
		(*it)->parent = this;
		if (!(*it)->transformOwner)
		{
			(*it)->transform = transform;
		}
		(*it)->parent = this;	
		(*it ++)->Draw();		
	}
}


void GameComponent::Cleanup()
{
	// Cleanup all the children
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		(*it ++)->Cleanup();	    	
	}
}



void GameComponent::Update(float timeDelta) {	
	// Update all the children
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{	
		if (!(*it)->alive)
		{
			it = children.erase(it);
		}
		else
		{
			(*it)->parent = this;
			(*it ++)->Update(timeDelta);
		}
	}

	// Only the transforms owner will calculate the transform. ALl the other components should 
	// just update the position and the quaternion
	if (transformOwner)
	{
		transform->Calculate();
	}
}

void GameComponent::Attach(shared_ptr<GameComponent> child)
{
	child->parent = this; 
	// All my children share the same transform if they dont already have one...
	if (child->transform == nullptr)
	{
		child->transform = transform; 
	}
	// Set up transform parenting
	if (transformOwner && child->transformOwner)
	{
		child->transform->parent = transform;
	}
	children.push_back(child);
}


std::list<std::shared_ptr<GameComponent>> * GameComponent::GetChildren()
{
	return & children;
}

shared_ptr<GameComponent> GameComponent::FindComponent(std::string tag)
{
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{	
		if ((*it)->tag == tag)
		{
			return * it;
		}
	}
	return nullptr;
}
