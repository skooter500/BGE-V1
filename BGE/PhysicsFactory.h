#pragma once
#include <memory>
#include "PhysicsComponent.h"
#include "GameComponent.h"

using namespace std;

namespace BGE
{
	class PhysicsFactory
	{
	private:
		btDiscreteDynamicsWorld * dynamicsWorld;
	public:
		PhysicsFactory(btDiscreteDynamicsWorld * dynamicsWorld);
		~PhysicsFactory(void);

		shared_ptr<PhysicsComponent> CreateBox(float width, float height, float depth, glm::vec3 pos, glm::quat quat);
		shared_ptr<PhysicsComponent> CreateSphere(float radius, glm::vec3 pos, glm::quat quat);
		shared_ptr<PhysicsComponent> CreateCylinder(float radius, float height, glm::vec3 pos, glm::quat quat);
		shared_ptr<PhysicsComponent> CreateVehicle(glm::vec3 pos);
		shared_ptr<PhysicsComponent> CreateCameraPhysics();
		shared_ptr<PhysicsComponent> CreateGroundPhysics();
	};
}
