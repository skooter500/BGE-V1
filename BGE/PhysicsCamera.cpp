#include "PhysicsCamera.h"
#include "Game.h"
#include "PhysicsGame1.h"
#include "Box.h"
#include <iostream>

using namespace BGE;

PhysicsCamera::PhysicsCamera():PhysicsComponent(NULL, NULL)
{
	elapsed = 10000.0f;
	fireRate = 5.0f;
	pickedUp = NULL;
}


PhysicsCamera::~PhysicsCamera(void)
{
}

void PhysicsCamera::getWorldTransform(btTransform &worldTrans) const
{
	worldTrans.setOrigin(GLToBtVector(parent->position));
	worldTrans.setRotation(GLToBtQuat(parent->orientation));
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
	PhysicsGame1 * game = (PhysicsGame1 * ) Game::Instance();

	float moveSpeed = speed;
	float timeToPass = 1.0f / fireRate;

	if ((keyState[SDL_SCANCODE_SPACE]) && (elapsed > timeToPass))
	{
		glm::vec3 pos = parent->position + (parent->look * 5.0f);
		Box * box = new Box(1,1,1);
		btCollisionShape * boxShape = new btBoxShape(btVector3(1, 1, 1) * 0.5);
		btScalar mass = 1;
		btVector3 fallInertia(0,0,0);
		boxShape->calculateLocalInertia(mass,fallInertia);

		box->position = pos;
		btQuaternion q = btQuaternion(RandomFloat(), RandomFloat(), RandomFloat());
		q.normalize();
		btDefaultMotionState * boxMotionState = new btDefaultMotionState(btTransform(q,btVector3(pos.x,pos.y,pos.z)));
			
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,boxMotionState, boxShape, fallInertia);
		btRigidBody * boxBody = new btRigidBody(fallRigidBodyCI);
		float force = 5000.0f;
		boxBody->applyCentralForce(GLToBtVector(parent->look) * force);
		PhysicsGame1 * game = (PhysicsGame1 * ) Game::Instance();
		game->dynamicsWorld->addRigidBody(boxBody);

		BGE::PhysicsComponent * boxComponent = new PhysicsComponent(boxShape, boxBody);
		boxComponent->scale = box->scale;
		box->AddChild(boxComponent);
		game->AddChild(box);
		boxBody->setUserPointer(box);
		elapsed = 0.0f;
	}
	else
	{
		elapsed += timeDelta;
	}

	// Handle the gravity gun
	if (SDL_GetMouseState(NULL, NULL) && SDL_BUTTON(3))
	{
		float dist = 10.0f;
		if (pickedUp == NULL)
		{		
			btVector3 rayFrom = GLToBtVector(parent->position + (parent->look * dist)); // Has to be some distance in front of the camera otherwise it will collide with the camera all the time
			btVector3 rayTo = GLToBtVector(parent->position + (parent->look * dist * dist));

			btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
			game->dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);
			
			if (rayCallback.hasHit())
			{
				pickedUp = reinterpret_cast<PhysicsComponent *>(rayCallback.m_collisionObject->getUserPointer());
				PhysicsComponent * groundComponent = (PhysicsComponent *) (* Game::Instance()->GetGround()->GetChildren()->begin());
				if (pickedUp == groundComponent)
				{
					pickedUp = NULL;
				}
			}
		}
		if (pickedUp != NULL)
		{
			printf("Picked something up!!\n");
            float powerfactor = 6.0f; // Higher values causes the targets moving faster to the holding point.
            float maxVel = 5.0f;      // Lower values prevent objects flying through walls.

            // Calculate the hold point in front of the camera
			glm::vec3 holdPos = parent->position + (parent->look * dist);

            glm::vec3 v = holdPos - pickedUp->position; // direction to move the Target
            v *= powerfactor; // powerfactor of the GravityGun

            if (v.length() > maxVel)
            {
                // if the correction-velocity is bigger than maximum
				v = glm::normalize(v);
                v *= maxVel; // just set correction-velocity to the maximum
            }
			pickedUp->rigidBody->setLinearVelocity(GLToBtVector(v));    
			pickedUp->rigidBody->activate();		
		}
	}
	else
	{
		printf("Picked nothing up!!\n");
            
		pickedUp = NULL;
	}
}

