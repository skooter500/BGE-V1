#pragma once
#include "GameComponent.h"
#include "Model.h"

using namespace std;
namespace BGE
{
	class Steerable3DController :
		public GameComponent
	{
	private:
		void Steerable3DController::CalculateInertiaTensor();
		shared_ptr<Model> model;
	public:
		Steerable3DController(shared_ptr<Model> model);
		~Steerable3DController(void);

		bool Initialise();
		void Update(float timeDelta);
		void Draw();

		float mass;
		glm::vec3 velocity, force, acceleration;
        glm::vec3 angularVelocity, angularAcceleration, torque;
        glm::mat3 inertialTensor;

		void AddForce(glm::vec3);
		void AddTorque(glm::vec3);
		void AddForceAtPoint(glm::vec3, glm::vec3);
	};
}
