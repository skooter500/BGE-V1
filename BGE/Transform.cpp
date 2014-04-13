#include "Transform.h"
#include "Utils.h"

using namespace BGE;

const glm::vec3 Transform::basisUp = glm::vec3(0, 1, 0);
const glm::vec3 Transform::basisLook = glm::vec3(0, 0, -1);
const glm::vec3 Transform::basisRight = glm::vec3(1, 0, 0);


Transform::Transform(void)
{
	position = glm::vec3(0, 0, 0); 
	look = glm::vec3(0, 0, -1); 
	right = glm::vec3(1, 0, 0); 
	up = glm::vec3(0, 1, 0); 
	velocity = glm::vec3(0, 0, 0); 
	world = glm::mat4(1.0f); // Identity
	orientation = glm::quat(); // Identity
	ambient = glm::vec3(0.5f, 0.5f, 0.5f);
	specular = glm::vec3(0.0f, 0.0f, 0.0f);
	moved = true;
	scale = glm::vec3(1.0, 1.0, 1.0);
	parent = nullptr; 
}


Transform::~Transform(void)
{
}

void Transform::RecalculateVectors()
{
	look = RotateVector(basisLook, orientation);
	right = RotateVector(basisRight, orientation);
	up = RotateVector(basisUp, orientation);
}

void Transform::Calculate()
{
	if (parent != nullptr) { 
		backupPosition = position; 
		backupOrientation = orientation; 
		//backupWorld = world; 
		
		//world = world * parent -> world; 
		orientation = parent -> orientation * orientation; 
		position = parent -> position + position; 
	}
	world = glm::translate(glm::mat4(1), position) * glm::mat4_cast(orientation) *  glm::scale(glm::mat4(1), scale);
	RecalculateVectors();
	/*if (parent != NULL)
	{
		world = (glm::translate(glm::mat4(1), transform->position) * glm::mat4_cast(parent->orientation)) * world;
	}
	*/
	if (parent != nullptr) { 
		position = backupPosition; 
		orientation = backupOrientation; 
	} 
}

void Transform::Walk(float units)
{
	position += look * units;
	moved = true;
}

void Transform::Strafe(float units)
{
	position += right * units;
	moved = true;
}

void Transform::Fly(float units)
{
	position.y += units;
	moved = true;
}

void Transform::Pitch(float angle, bool limitRotation)
{
	float invcosTheta1 = glm::dot(look, basisUp);
	float threshold = 0.95f;
	if ((limitRotation) && ((angle < 0 && invcosTheta1 < (-threshold)) || (angle > 0 && invcosTheta1 > (threshold)))) 
	{
		return;
	}
	
	// A pitch is a rotation around the right vector
	glm::quat rot = glm::angleAxis(angle, right);

	orientation = rot * orientation;
	
	/*
	glm::mat4 pitch;
	pitch = glm::rotate(pitch, angle, transform->right);

	glm::vec4 tlook = glm::vec4(look, 0);
	tlook = pitch * tlook;
	look = glm::vec3(tlook);

	glm::vec4 tup = glm::vec4(up, 0);
	tlook = pitch * tup;
	up = glm::vec3(tup);
	*/
	moved = true;
}

void Transform::Yaw(float angle)
{
	// A yaw is a rotation around the global up vector
	glm::quat rot = glm::angleAxis(angle, Transform::basisUp);

	orientation = rot * orientation;

	/*
	glm::mat4 yaw;
	yaw = glm::rotate(yaw, angle, Transform::basisUp); 

	glm::vec4 tlook = glm::vec4(look, 0);
	tlook = yaw * tlook;
	look = glm::vec3(tlook);

	glm::vec4 tright = glm::vec4(transform->right, 0);
	tright = yaw * tright;
	transform->right = glm::vec3(tright);
	*/
	moved = true;
}

void Transform::RotateVectors()
{
	look = orientation * look;
	right = orientation * right;
	up = orientation * up;

}

void Transform::Roll(float angle)
{
	// A roll is a rotation around the look vector
	glm::quat rot = glm::angleAxis(angle, look);

	orientation = rot * orientation;
	moved = true;
}
