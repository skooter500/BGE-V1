#include "Ship.h"
#include "Content.h"
#include "Model.h"
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>

using namespace BGE;

Ship::Ship(void):GameComponent()
{
	mass = 10.0f;
	velocity = glm::vec3(0);
	force = glm::vec3(0);
	acceleration = glm::vec3(0);
	angularAcceleration = glm::vec3(0);
	angularVelocity = glm::vec3(0);
	torque = glm::vec3(0);
}


Ship::~Ship(void)
{
}

bool Ship::Initialise()
{
	model = Content::LoadModel("cobramk3");
	AddChild(model);

	GameComponent::Initialise();
	return true;
}

void Ship::CalculateInertiaTensor() { 
	float width = model->boundingBox.max.x - model->boundingBox.min.x;
	float height = model->boundingBox.max.y - model->boundingBox.min.y;
	float depth = model->boundingBox.max.z - model->boundingBox.min.z;

	inertialTensor[0][0] = (float) (mass * (pow(height, 2) + pow(depth, 2))) / 12.0f;
    inertialTensor[1][1] = (float) (mass * (pow(width, 2) + pow(depth, 2))) / 12.0f;
    inertialTensor[2][2] = (float) (mass * (pow(width, 2) + pow(height, 2))) / 12.0f;
}

void Ship::AddForce(glm::vec3 force)
{
    this->force += force;
}

void Ship::AddTorque(glm::vec3 torque)
{
    this->torque += torque;
}

void Ship::AddForceAtPoint(glm::vec3 force, glm::vec3 point)
{
	glm::vec3 to = glm::cross(force, point);
    torque += to;

    force += force;
}


void Ship::Update(float timeDelta)
{
	const Uint8 * keyState = Game::Instance()->GetKeyState();

	if (keyState[SDL_SCANCODE_SPACE])
    {
        AddForce(look * 10.0f);
    }

    // Yaw
	if (keyState[SDL_SCANCODE_J])
    {
		AddTorque(GameComponent::basisUp * 10.0f);
    }
    if (keyState[SDL_SCANCODE_L])
    {
        AddTorque(GameComponent::basisUp * -10.0f);
    }
    // End of Yaw

    //Pitch
    if (keyState[SDL_SCANCODE_I])
    {
        AddTorque(GameComponent::basisRight * 10.0f);
    }
    if (keyState[SDL_SCANCODE_K])
    {
        AddTorque(GameComponent::basisRight * -10.0f);
    }
    // End of Pitch

	// Roll
    if (keyState[SDL_SCANCODE_Y])
    {
        AddTorque(GameComponent::basisLook * 10.0f);
    }

    if (keyState[SDL_SCANCODE_H])
    {
        AddTorque(GameComponent::basisLook * -10.0f);
    }

    // Do the Newtonian integration
    acceleration = force / mass;
    velocity += acceleration * timeDelta;
    position += velocity * timeDelta;
    force = glm::vec3(0);

	
    if (glm::length(velocity) > 0.0001f)
    {
		look = glm::normalize(velocity);
        right = glm::cross(look, up);
        velocity *= 0.99f;
    }

    // Do the Hamiltonian integration
	angularAcceleration = torque * glm::inverse(inertialTensor);
    angularVelocity = angularVelocity + (angularAcceleration * timeDelta);

    glm::quat w = glm::quat(angularVelocity.x, angularVelocity.y, angularVelocity.z, 0);

	orientation = orientation + ((w * (timeDelta / 2.0f)) * glm::inverse(orientation));
	orientation = glm::normalize(orientation);
    torque = glm::vec3(0);

	GameComponent::Update(timeDelta);

	/*
    // Recalculate the Look, up and right
    w = glm::quat(GameComponent::basisLook, 0);
    w = quaternion * w * Quaternion.Inverse(quaternion);
    look.x = w.X;
    look.y = w.Y;
    look.z = w.Z;
    w = glm::quat(Vector3.Up, 0);
    w = quaternion * w * Quaternion.Inverse(quaternion);
    up.X = w.X;
    up.Y = w.Y;
    up.Z = w.Z;

    w = new Quaternion(GameComponent::basisRight, 0);
    w = quaternion * w * Quaternion.Inverse(quaternion);
    right.X = w.X;
    right.Y = w.Y;
    right.Z = w.Z;
	*/

	GameComponent::Update(timeDelta);
}
