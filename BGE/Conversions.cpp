#pragma once

#include "Conversions.h"

using namespace FMOD;
using namespace OVR;

FMOD_VECTOR BGE::GLToFMODVector(glm::vec3 v)
{
	FMOD_VECTOR fv;
	fv.x = v.x;
	fv.y = v.y;
	fv.z = v.z;
	return fv;
}

glm::quat BGE::OVRToGLQuat(Quatf q)
{
	return glm::quat(q.w, q.x, q.y, q.z);
}

OVR::Vector3f BGE::GLToOVRVector(glm::vec3 v)
{
	return OVR::Vector3f(v.x, v.y, v.z);
}


OVR::Matrix4f BGE::GLToOVRMat4(glm::mat4 m)
{
	m = glm::transpose(m);
	OVR::Matrix4f ret;
	memcpy(& ret, & m, sizeof(OVR::Matrix4f));
	return ret;
}

glm::mat4 BGE::OVRToGLMat4(OVR::Matrix4f m)
{
	glm::mat4 ret;
	m.Transpose();
	memcpy(& ret, & m, sizeof(OVR::Matrix4f));
	return ret;
}

glm::vec3 BGE::BtToGLVector(const btVector3 & v)
{
	return glm::vec3(v.getX(), v.getY(), v.getZ());
}

glm::quat BGE::BtToGLQuat(const btQuaternion & q)
{
	return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
}

btVector3 BGE::GLToBtVector(const glm::vec3 & v)
{
	return btVector3(v.x, v.y, v.z);
}

btQuaternion BGE::GLToBtQuat(const glm::quat & q)
{
	return  btQuaternion(q.x, q.y, q.z, q.w);
}

