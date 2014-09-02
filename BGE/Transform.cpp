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
	world = glm::mat4(1.0f); // Identity
	orientation = glm::quat(); // Identity
	diffuse = ambient = specular = glm::vec3(0.0f, 0.0f, 0.0f);
	moved = true;
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	parent = nullptr; 
}

Transform::~Transform(void)
{
	parent = nullptr;
}

void Transform::RecalculateVectors()
{
	look = RotateVector(basisLook, orientation);
	right = RotateVector(basisRight, orientation);
	up = RotateVector(basisUp, orientation);
}

void Transform::Calculate()
{
	worldNoScale = glm::translate(glm::mat4(1), position) * glm::mat4_cast(orientation);
	world = worldNoScale * glm::scale(glm::mat4(1), scale);
	RecalculateVectors();
	if (parent != nullptr)
	{
		world = parent->worldNoScale * world;
		worldNoScale = parent->worldNoScale * worldNoScale;
	}
}

glm::vec3 Transform::TransformPosition(glm::vec3 in, bool scale)
{
	Transform::Calculate();
	if (scale)
	{
		return glm::vec3(world * glm::vec4(in, 1.0f));
	}
	else
	{
		return glm::vec3(worldNoScale * glm::vec4(in, 1.0f));
	}
}

glm::vec3 Transform::TransformNormal(glm::vec3 in, bool scale)
{
	if (scale)
	{
		return glm::vec3(world * glm::vec4(in, 0.0f));
	}
	else
	{
		return glm::vec3(worldNoScale * glm::vec4(in, 0.0f));
	}
}

glm::vec3 Transform::InverseTransformNormal(glm::vec3 in, bool scale)
{
	if (scale)
	{
		return glm::vec3(glm::inverse(world) * glm::vec4(in, 0.0f));
	}
	else
	{
		return glm::vec3(glm::inverse(worldNoScale) * glm::vec4(in, 0.0f));
	}
}

glm::vec3 Transform::InverseTransformPosition(glm::vec3 in, bool scale)
{
	if (scale)
	{
		return glm::vec3(glm::inverse(world) * glm::vec4(in, 1.0f));
	}
	else
	{
		return glm::vec3(glm::inverse(worldNoScale) * glm::vec4(in, 1.0f));
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

glm::quat BGE::Transform::TransformOrientation(glm::quat in)
{
	Transform::Calculate();
	glm::quat rot = glm::quat_cast(world);
	glm::quat q = rot * q;
	return q;
}

glm::quat BGE::Transform::InverseTransformOrientation(glm::quat in)
{
	Transform::Calculate();
	glm::quat rot = glm::quat_cast(glm::inverse(world));
	glm::quat q = rot * q;
	return q;
}
