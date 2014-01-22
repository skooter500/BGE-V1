#include "GameComponent.h"
#include "Model.h"
#include "Game.h"
#include <iostream>
#include <cmath>
#include <gtx\constants.hpp>
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
	alive = true;
	initialised = false;
}


bool GameComponent::Initialise()
{	
	// Initialise all the children
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		(*it ++)->initialised = (*it)->Initialise();	
	}
	return true;
}

void GameComponent::Draw()
{
	// Draw all the children
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
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
	
	transform->Calculate();
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
			(*it ++)->Update(timeDelta);
		}
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
	children.push_back(child);
}


std::list<std::shared_ptr<GameComponent>> * GameComponent::GetChildren()
{
	return & children;
}

shared_ptr<GameComponent> GameComponent::GetController()
{
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{	
		if ((*it)->worldMode == world_modes::to_parent)
		{
			return * it;
		}
		it ++;
	}
	throw new BGE::Exception("Tried to get a controller for a game component, but none exists");
}