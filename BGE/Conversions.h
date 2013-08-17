#pragma once

#include<glm.hpp>
#include<fmod.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <btBulletDynamicsCommon.h>
#include <OVR.h>

using namespace FMOD;
using namespace OVR;

namespace BGE
{

	FMOD_VECTOR GLToFMODVector(glm::vec3 v);
	glm::quat OVRToGLQuat(Quatf q);
	OVR::Vector3f GLToOVRVector(glm::vec3 v);
	OVR::Matrix4f GLToOVRMat4(glm::mat4 m);
	glm::mat4 OVRToGLMat4(OVR::Matrix4f m);
	glm::vec3 BtToGLVector(const btVector3 & v);
	glm::quat BtToGLQuat(const btQuaternion & q);
	btVector3 GLToBtVector(const glm::vec3 & v);
	btQuaternion GLToBtQuat(const glm::quat & q);
}
