#include "Steerable3DController.h"
#include "Content.h"
#include "Model.h"
#include "Utils.h"
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>
#include <sstream>
#include <string>

using namespace BGE;
using namespace std;

Steerable3DController::Steerable3DController(shared_ptr<Model> model):GameComponent()
{
	mass = 10.0f;
	velocity = glm::vec3(0);
	force = glm::vec3(0);
	acceleration = glm::vec3(0);
	angularAcceleration = glm::vec3(0);
	angularVelocity = glm::vec3(0);
	torque = glm::vec3(0);
	this->model = model;
}

Steerable3DController::~Steerable3DController(void)
{
}

bool Steerable3DController::Initialise()
{
	if (!model->initialised)
	{
		model->Initialise();
	}
	CalculateInertiaTensor();
	
	GameComponent::Initialise();
	return true;
}

void Steerable3DController::CalculateInertiaTensor() { 
	float width = model->boundingBox.max.x - model->boundingBox.min.x;
	float height = model->boundingBox.max.y - model->boundingBox.min.y;
	float depth = model->boundingBox.max.z - model->boundingBox.min.z;

	inertialTensor[0][0] = (float) (mass * (pow(height, 2) + pow(depth, 2))) / 12.0f;
    inertialTensor[1][1] = (float) (mass * (pow(width, 2) + pow(depth, 2))) / 12.0f;
    inertialTensor[2][2] = (float) (mass * (pow(width, 2) + pow(height, 2))) / 12.0f;
}

void Steerable3DController::AddForce(glm::vec3 force)
{
    this->force += force;
}

void Steerable3DController::AddTorque(glm::vec3 torque)
{
    this->torque += torque;
}

void Steerable3DController::AddForceAtPoint(glm::vec3 force, glm::vec3 point)
{
	glm::vec3 to = glm::cross(force, point);
    torque += to;

    force += force;
}

void Steerable3DController::Draw()
{
	GameComponent::Draw();
}


void Steerable3DController::Update(float timeDelta)
{
	const Uint8 * keyState = Game::Instance()->GetKeyState();

	float scale = 10000.0f;
	if (keyState[SDL_SCANCODE_SPACE])
    {
        AddForce(transform->look * scale * timeDelta);
    }

    // Yaw
	if (keyState[SDL_SCANCODE_J])
    {
		AddTorque(transform->up * scale * timeDelta);
    }
    if (keyState[SDL_SCANCODE_L])
    {
        AddTorque(- transform->up * scale * timeDelta);
    }
    // End of Yaw

    //Pitch
    if (keyState[SDL_SCANCODE_I])
    {
        AddTorque(transform->right * scale * timeDelta);
    }
    if (keyState[SDL_SCANCODE_K])
    {
        AddTorque(- transform->right * scale * timeDelta);
    }
    // End of Pitch

	// Roll
    if (keyState[SDL_SCANCODE_Y])
    {
        AddTorque(transform->look * scale * timeDelta);
    }

    if (keyState[SDL_SCANCODE_H])
    {
        AddTorque(- transform->look * scale * timeDelta);
    }

    // Do the Newtonian integration
    acceleration = force / mass;
    transform->velocity += acceleration * timeDelta;
    transform->position += velocity * timeDelta;
	
	// Normalise the velocity into the look
	// Probably not necessary as we recalculate these anyway later
    if (glm::length(velocity) > 0.0001f)
    {
		transform->look = glm::normalize(velocity);
        transform->right = glm::cross(transform->look, transform->up);
        velocity *= 0.99f;
    }

    // Do the Hamiltonian integration
	angularAcceleration = torque * glm::inverse(inertialTensor);
    angularVelocity = angularVelocity + angularAcceleration * timeDelta;

    glm::quat w = glm::quat(0, angularVelocity.x, angularVelocity.y, angularVelocity.z);

	transform->orientation = transform->orientation + ((w * (timeDelta / 2.0f)) * transform->orientation);
	transform->orientation = glm::normalize(transform->orientation);
    
	// Reset the accumulators
	torque = glm::vec3(0);
	force = glm::vec3(0);

	transform->look = RotateVector(Transform::basisLook, transform->orientation);
	transform->up = RotateVector(Transform::basisUp, transform->orientation);
	transform->right = RotateVector(Transform::basisRight, transform->orientation);

	GameComponent::Update(timeDelta);
}
