#include "PhysicsCamera.h"
#include "Game.h"
#include "PhysicsGame1.h"
#include "Box.h"
#include <iostream>
#include <sstream>
#include "Utils.h"
using namespace BGE;

PhysicsCamera::PhysicsCamera(PhysicsFactory * physicsFactory):PhysicsController()
{
	elapsed = 10000.0f;
	fireRate = 5.0f;
	pickedUp = NULL;
	tag = "Physics Camera";
	this->physicsFactory = physicsFactory;
}


PhysicsCamera::~PhysicsCamera(void)
{
}

void PhysicsCamera::getWorldTransform(btTransform &worldTrans) const
{
	worldTrans.setOrigin(GLToBtVector(transform->position));
	worldTrans.setRotation(GLToBtQuat(transform->orientation));
}

void PhysicsCamera::setWorldTransform(const btTransform &worldTrans)
{
	// Should never get called, because this body is kinematic??
}

void PhysicsCamera::Update(float timeDelta)
{
	// Override the one in the base class, we do not want to update our world transform from the physics object
	// WHich is what the one in the base class does...

	const Uint8 * keyState = Game::Instance()->GetKeyState();
	shared_ptr<Game> game = Game::Instance();

	float moveSpeed = speed;
	float timeToPass = 1.0f / fireRate;
	if ((keyState[SDL_SCANCODE_SPACE]) && (elapsed > timeToPass))
	{
		glm::vec3 pos = transform->position + (transform->look * 5.0f);
		glm::quat q(RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat());
		glm::normalize(q);
		shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateBox(1,1,1, pos, q);
		
		float force = 5000.0f;
		physicsComponent->rigidBody->applyCentralForce(GLToBtVector(transform->look) * force);
		elapsed = 0.0f;
	}
	else
	{
		elapsed += timeDelta;
	}
	string what = "Nothing";
	// Handle the gravity gun
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		float dist = 1000.0f;
		if (pickedUp == NULL)
		{		
			btVector3 rayFrom = GLToBtVector(transform->position + (transform->look * 4.0f)); // Has to be some distance in front of the camera otherwise it will collide with the camera all the time
			btVector3 rayTo = GLToBtVector(transform->position + (transform->look * dist));

			btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
			physicsFactory->dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);
			
			if (rayCallback.hasHit())
			{
				pickedUp = reinterpret_cast<PhysicsController*>(rayCallback.m_collisionObject->getUserPointer());
				if (pickedUp->parent == game->GetGround())
				{
					pickedUp = NULL;
				}
			}
		}
		if (pickedUp != NULL)
		{
			float powerfactor = 4.0f; // Higher values causes the targets moving faster to the holding point.
            float maxVel = 3.0f;      // Lower values prevent objects flying through walls.
			float holdDist = 6.0f;

            // Calculate the hold point in front of the camera
			glm::vec3 holdPos = transform->position + (transform->look * holdDist);

            glm::vec3 v = holdPos - pickedUp->transform->position; // direction to move the Target
            v *= powerfactor; // powerfactor of the GravityGun

            if (v.length() > maxVel)
            {
                // if the correction-velocity is bigger than maximum
				v = glm::normalize(v);
                v *= maxVel; // just set correction-velocity to the maximum
            }
			pickedUp->rigidBody->setLinearVelocity(GLToBtVector(v));    
			pickedUp->rigidBody->activate();		
			what = pickedUp->tag;	
		}
	}
	else
	{    
		pickedUp = NULL;
	}
	stringstream ss;
	ss << "Picked up: " << what;
	game->PrintText(ss.str());
}

