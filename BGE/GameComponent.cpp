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
	glm::vec3 parentPos; 
	/*if (isRelative) { 
		// If this is a relativly positioned child, transform it to be positoned in it's parent's local space 
		float angle = glm::dot(transform -> look, Transform::basisLook); 
		glm::vec3 rotationAxis = glm::cross(transform -> look, Transform::basisLook); 
		glm::quat quaternion = glm::angleAxis(angle, rotationAxis); 
		transform -> orientation = transform -> orientation * quaternion; 
		transform -> Calculate(); 
		transform -> RotateVectors(); 
	}*/
	while (it != children.end())
	{
		/*if (isRelative && (*it)->tag == "Model") { 
			shared_ptr<Model> im = static_pointer_cast<Model>(*it); 
			im -> localTransform = glm::translate(glm::mat4(1), relativeTransform->position); // Add new local transform to the model 
		}*/
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
		} else {
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
	child -> transform -> parent = transform.get(); 
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
