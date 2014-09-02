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

GameComponent::~GameComponent()
{
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
			component->ClearAllChildren();
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
		component->ClearAllChildren();
		++it;
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
		(*it)->parent = This();
		if (!(*it)->transformOwner)
		{
			(*it)->transform = transform;
		}
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
		(*it)->parent = This();
		if (!(*it)->transformOwner)
		{
			(*it)->transform = transform;
		}
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
		(*it)->parent = This();
		if (!(*it)->transformOwner)
		{
			(*it)->transform = transform;
		}
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

void GameComponent::SetAlive(bool alive)
{
	this->alive = alive;
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		(*it)->alive = alive;
	}
}

void GameComponent::Update(float timeDelta) {	
	// Update all the children
	std::list<std::shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{	
		shared_ptr<GameComponent> current = *it;
		if (!alive)
		{
			current->alive = false;
		}
		current->parent = This();
		current->Update(timeDelta);
		if (!current->alive)
		{
			current->parent = nullptr;
			it = children.erase(it);
		}
		else
		{
			++it;
		}
	}

	multimap<string, shared_ptr<GameComponent>>::iterator mit = childrenMap.begin();
	while (mit != childrenMap.end())
	{
		if (!(*mit).second->alive)
		{
			shared_ptr<GameComponent> g = mit->second;
			mit = childrenMap.erase(mit);

		}
		else
		{
			++mit;
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
	child->parent = This();
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
	std::multimap<std::string, std::shared_ptr<GameComponent>>::iterator it = childrenMap.find(tag);
	if (it != childrenMap.end())
	{
		return it->second;
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

std::shared_ptr<GameComponent> GameComponent::This()
{
	return shared_from_this();
}

void BGE::GameComponent::TransformChildren(shared_ptr<Transform> childTransform)
{
	// Transform all the bones by the handTransform
	list<shared_ptr<GameComponent>>::iterator it = children.begin();
	while (it != children.end())
	{
		shared_ptr<GameComponent> child = *it;
		child->transform->position = childTransform->TransformPosition(child->transform->position, false);
		//child->transform->orientation = childTransform->TransformOrientation(child->transform->orientation);
		it++;
	}
}


