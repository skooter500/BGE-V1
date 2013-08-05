#include "Ship.h"
#include "Content.h"
#include "Model.h"
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>

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

	drawMode = draw_modes::materials;
	ambient = glm::vec3(0.2f, 0.2, 0.2f);
	specular = glm::vec3(1.2f, 1.2f, 1.2f);
}


Ship::~Ship(void)
{
}

bool Ship::Initialise()
{
	model = Content::LoadModel("cobramk3");
	children.push_back(model);

	AddChild(model);	
	GameComponent::Initialise();
	CalculateInertiaTensor();
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

void Ship::Draw()
{
	glm::mat4 yaw;
	// Necessary because the model points the wrong way!
	yaw = glm::rotate(yaw, 180.0f, basisUp);
	world = world * yaw;
	GameComponent::Draw();
}


void Ship::Update(float timeDelta)
{
	const Uint8 * keyState = Game::Instance()->GetKeyState();

	float scale = 10000.0f;
	if (keyState[SDL_SCANCODE_SPACE])
    {
        AddForce(look * scale * timeDelta);
    }

    // Yaw
	if (keyState[SDL_SCANCODE_J])
    {
		AddTorque(up * scale * timeDelta);
    }
    if (keyState[SDL_SCANCODE_L])
    {
        AddTorque(- up * scale * timeDelta);
    }
    // End of Yaw

    //Pitch
    if (keyState[SDL_SCANCODE_I])
    {
        AddTorque(right * scale * timeDelta);
    }
    if (keyState[SDL_SCANCODE_K])
    {
        AddTorque(- right * scale * timeDelta);
    }
    // End of Pitch

	// Roll
    if (keyState[SDL_SCANCODE_Y])
    {
        AddTorque(look * scale * timeDelta);
    }

    if (keyState[SDL_SCANCODE_H])
    {
        AddTorque(- look * scale * timeDelta);
    }

    // Do the Newtonian integration
    acceleration = force / mass;
    velocity += acceleration * timeDelta;
    position += velocity * timeDelta;
	
    if (glm::length(velocity) > 0.0001f)
    {
		look = glm::normalize(velocity);
        right = glm::cross(look, up);
        velocity *= 0.99f;
    }

    // Do the Hamiltonian integration
	angularAcceleration = torque * glm::inverse(inertialTensor);
    angularVelocity = angularVelocity + angularAcceleration * timeDelta;

    glm::quat w = glm::quat(0, angularVelocity.x, angularVelocity.y, angularVelocity.z);

	orientation = orientation + ((w * (timeDelta / 2.0f)) * orientation);
	orientation = glm::normalize(orientation);
    torque = glm::vec3(0);
	force = glm::vec3(0);

	look = RotateVector(basisLook, orientation);
	up = RotateVector(basisUp, orientation);
	right = RotateVector(basisRight, orientation);

	GameComponent::Update(timeDelta);
}
