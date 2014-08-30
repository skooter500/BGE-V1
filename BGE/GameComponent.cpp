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

int GameComponent::ClearChildrenWithTag(string tag)
{
	int count = 0;
	list<shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		shared_ptr<GameComponent> child = * it;
		if (child->tag == tag)
		{
			shared_ptr<GameComponent> component = * it;
			component->alive = false;
			count++;
		}
		it++;
	}
	return count;
}

int GameComponent::ClearAllChildren()
{
	int count = 0;
	list<shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		shared_ptr<GameComponent> component = *it;		
		component->alive = false;
		it++;
		count++;
	}
	return count;
}

bool GameComponent::Initialise()
{	
	// Initialise all the children
	
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	
	try
	{
		while (it != children.end())
		{
			(*it)->initialised = (*it)->Initialise();
			*it++;
		}
	}
	catch (BGE::Exception e)
	{
		fprintf(stdout, e.What());
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
		(*it)->parent = getptr();
		if (!(*it)->transformOwner)
		{
			(*it)->transform = transform;
		}
		(*it)->parent = getptr();
		(*it ++)->Draw();		
	}
}

void GameComponent::PreDraw()
{
	// Draw all the children
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		// This is necessary for models etc that are instanced
		// As they may be attached to several different parents
		(*it)->parent = getptr();
		if (!(*it)->transformOwner)
		{
			(*it)->transform = transform;
		}
		(*it)->parent = getptr();
		(*it++)->PreDraw();
	}
}

void GameComponent::PostDraw()
{
	// Draw all the children
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		// This is necessary for models etc that are instanced
		// As they may be attached to several different parents
		(*it)->parent = getptr();
		if (!(*it)->transformOwner)
		{
			(*it)->transform = transform;
		}
		(*it)->parent = getptr();
		(*it++)->PostDraw();
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
			(*it)->parent = getptr();
			(*it ++)->Update(timeDelta);
		}
	}

	multimap<string, shared_ptr<GameComponent>>::iterator mit = childrenMap.begin();
	while (it != children.end())
	{
		if (!(*it)->alive)
		{
			it = children.erase(it);
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
	child->parent = getptr();
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
	childrenMap.insert(std::pair<std::string, shared_ptr<GameComponent>>(child->tag, child));
}


std::list<std::shared_ptr<GameComponent>> * GameComponent::GetChildren()
{
	return & children;
}

shared_ptr<GameComponent> GameComponent::FindComponentByTag(std::string tag)
{
	std::map<std::string, std::shared_ptr<GameComponent>>::iterator it = childrenMap.find(tag);
	if (it != childrenMap.end())
	{
		return (*it).second;
	}
	else
	{
		return nullptr;
	}
	/*std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		if ((*it)->tag == tag)
		{
			return *it;
		}
		it++;
	}
	return nullptr;*/
}

std::vector<std::shared_ptr<GameComponent>> GameComponent::FindComponentsByTag(std::string tag)
{
	std::map<std::string, std::shared_ptr<GameComponent>>::iterator it = childrenMap.find(tag);	
	std::vector<std::shared_ptr<GameComponent>> components;
	while (it != childrenMap.end())
	{
		components.push_back((*it).second);		
		it++;
	}
	return components;
}

std::shared_ptr<GameComponent> GameComponent::getptr()
{
	return shared_from_this();
}


