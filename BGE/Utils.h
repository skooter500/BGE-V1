#pragma once

#include<glm.hpp>
#include<fmod.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <btBulletDynamicsCommon.h>
#include <Kinect.h>
#ifdef _WIN32
 #include <OVR.h>
#endif

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <string>
#include <Leap.h>

using namespace FMOD;
using namespace std;

#define SAFE_DELETE(a) if( (a) != nullptr ) delete (a); (a) = nullptr;

namespace BGE
{
	struct Material
	{
		glm::vec3 diffuse;
		string textureName;
	};
	struct RayGeom
	{
		glm::vec3 pos;
		glm::vec3 look;
	};

	struct SphereGeom
	{
		glm::vec3 pos;
		float radius;
	};

	FMOD_VECTOR GLToFMODVector(glm::vec3 v);
#ifdef _WIN32
	glm::quat OVRToGLQuat(OVR::Quatf q);
	ovrVector3f GLToOVRVector(glm::vec3 v);
	ovrMatrix4f GLToOVRMat4(glm::mat4 m);
	glm::mat4 OVRToGLMat4(OVR::Matrix4f m);
	glm::vec3 OVRToGLVector(OVR::Vector3f v);
#endif
	glm::vec3 BtToGLVector(const btVector3 & v);
	glm::quat BtToGLQuat(const btQuaternion & q);
	btVector3 GLToBtVector(const glm::vec3 & v);
	btQuaternion GLToBtQuat(const glm::quat & q);

	glm::vec3 KinectToGLVector(CameraSpacePoint v);

	// Leap Stuff
	glm::vec3 LeapToGlVec3(Leap::Vector v);



	string ltrim(string s);
	string rtrim(string s);
	string trim(string s);

	void LogMessage(string message);

	bool CheckNaN(glm::vec3 & v);
	bool CheckNaN(glm::vec3  & v, const glm::vec3 & def);
	float Clip(float x, float min, float max);
	float Interpolate(float alpha, float x0, float x1);
	glm::vec3 Interpolate(float alpha, glm::vec3 x0, glm::vec3 x1);
	void BlendIntoAccumulator(float smoothRate, float newValue, float & smoothedAccumulator);
	void BlendIntoAccumulator(float smoothRate, glm::vec3 newValue, glm::vec3 & smoothedAccumulator);

	glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
	float RandomClamped(float min = -1.0f, float max = 1.0f);
	glm::vec3 RandomInsideUnitSphere();
	glm::vec3 RandomPosition(float range);

	bool ClosestRayIntersectsSphere(const RayGeom & ray, const SphereGeom & sphere, const glm::vec3 & point, glm::vec3 & intersection);

	glm::vec3 RotateVector(glm::vec3, glm::quat);
	void CheckOverflow(int & x);
	void SafeDelete(void ** p);
}
