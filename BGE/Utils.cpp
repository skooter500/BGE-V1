#pragma once

#include "Utils.h"
#include <gtx/norm.hpp>
using namespace FMOD;
using namespace std;

float BGE::RandomClamped(float min, float max)
{
	float r = (float)rand()/(float)RAND_MAX;
	float range = max - min;
	return max - (r * range); 
}

glm::vec3 BGE::KinectToGLVector(CameraSpacePoint v)
{
	return glm::vec3(v.X, v.Y, v.Z);
}

FMOD_VECTOR BGE::GLToFMODVector(glm::vec3 v)
{
	FMOD_VECTOR fv;
	fv.x = v.x;
	fv.y = v.y;
	fv.z = v.z;
	return fv;
}

#ifdef _WIN32
glm::quat BGE::OVRToGLQuat(OVR::Quatf q)
{
	return glm::quat(q.w, q.x, q.y, q.z);
}

ovrVector3f BGE::GLToOVRVector(glm::vec3 v)
{
	return OVR::Vector3f(v.x, v.y, v.z);
}

ovrMatrix4f BGE::GLToOVRMat4(glm::mat4 m)
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
#endif 

glm::vec3 BGE::BtToGLVector(const btVector3 & v)
{
	return glm::vec3(v.getX(), v.getY(), v.getZ());
}

glm::quat BGE::BtToGLQuat(const btQuaternion & q)
{
	return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
}

glm::vec3 BGE::OVRToGLVector(OVR::Vector3f v)
{
	return glm::vec3(v.x, v.y, v.z);
}

btVector3 BGE::GLToBtVector(const glm::vec3 & v)
{
	return btVector3(v.x, v.y, v.z);
}

btQuaternion BGE::GLToBtQuat(const glm::quat & q)
{
	return  btQuaternion(q.x, q.y, q.z, q.w);
}

// trim from start
string BGE::ltrim(string s) 
{
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	return s;
}

// trim from end
string BGE::rtrim(string s) 
{
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
	return s;
}

// trim from both ends
string BGE::trim(string s) 
{
	return BGE::ltrim(BGE::rtrim(s));
}

void BGE::LogMessage(string message)
{
	printf("%s\n", message.c_str());
}

bool BGE::CheckNaN(glm::vec3  & v, const glm::vec3 & def)
{
	if (glm::isnan(v.x))
	{
		LogMessage("Nan");
		v = def;
		return true;
	}
	if (glm::isnan(v.y))
	{
		LogMessage("Nan");
		v = def;
		return true;
	}
	if (glm::isnan(v.z))
	{
		LogMessage("Nan");
		v = def;
		return true;
	}
	return false;
}

bool BGE::CheckNaN(glm::vec3 & v)
{
	if (glm::isnan(v.x))
	{
		LogMessage("Nan");
		return true;
	}
	if (glm::isnan(v.y))
	{
		LogMessage("Nan");
		return true;
	}
	if (glm::isnan(v.z))
	{
		LogMessage("Nan");
		return true;
	}
	return false;
}

float BGE::Clip(float x, float min, float max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

float BGE::Interpolate(float alpha, float x0, float x1)
{
	return x0 + ((x1 - x0) * alpha);
}

glm::vec3 BGE::Interpolate(float alpha, glm::vec3 x0, glm::vec3 x1)
{
	return x0 + ((x1 - x0) * alpha);
}

void BGE::BlendIntoAccumulator(float smoothRate, float newValue, float & smoothedAccumulator)
{
	smoothedAccumulator = Interpolate(Clip(smoothRate, 0, 1), smoothedAccumulator, newValue);
}

void BGE::BlendIntoAccumulator(float smoothRate, glm::vec3 newValue, glm::vec3 & smoothedAccumulator)
{
	smoothedAccumulator = Interpolate(Clip(smoothRate, 0, 1), smoothedAccumulator, newValue);
}

glm::quat BGE::RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f)
	{
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = glm::normalize(rotationAxis);
		return glm::angleAxis(180.0f, rotationAxis);
	}

	rotationAxis = glm::cross(start, dest);

	float s = glm::sqrt( (1+cosTheta)*2 );
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f, 
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
		);
}

glm::vec3 BGE::RandomPosition(float range)
{
	glm::vec3 pos;
	pos.x = RandomClamped() * range - (range /2.0f);
	pos.y = RandomClamped() * range - (range /2.0f);
	pos.z = RandomClamped() * range - (range /2.0f);
	return pos;
}

bool BGE::ClosestRayIntersectsSphere(const RayGeom & ray, const SphereGeom & sphere, const glm::vec3 & point, glm::vec3 & intersection)
{
    // Calculate p0-c call it v

    glm::vec3 v = ray.pos - sphere.pos;
    glm::vec3 p0 = glm::vec3(0), p1 = glm::vec3(0);

    // Now calculate a, b and c
    float a, b, c;

    /*
        *  a = u.u
        b = 2u(p0 – pc)
        c = (p0 – c).(p0 – c) - r2
    */
    a = glm::dot(ray.look, ray.look);
    b = 2.0f * glm::dot(v, ray.look);
    c = glm::dot(v, v) - (sphere.radius * sphere.radius);

    // Calculate the discriminant
    float discriminant = (b * b) - (4.0f * a * c);

    // Test for imaginary number
    // If discriminant > 0, calculate values for t0 and t1
    // Substitute into the ray equation and calculate the 2 intersection points
    // Push the interesctions into the vector intersections
    if (discriminant >= 0.0f)
    {

        discriminant = (float) glm::sqrt(discriminant);

        float t0 = (-b + discriminant) / (2.0f * a);
        float t1 = (-b - discriminant) / (2.0f * a);

        p0 = ray.pos + (ray.look * t0);
        p1 = ray.pos + (ray.look * t1);

        if (glm::length(point - p0) < glm::length(point - p1))
        {
            intersection = p0;
        }
        else
        {
            intersection = p1;
        }
        return true;
    }
    return false;
}

glm::vec3 BGE::RandomInsideUnitSphere()
{
	glm::vec3 ret;
	int halfRandMax = RAND_MAX / 2;
	ret.x = rand() - halfRandMax;
	ret.y = rand() - halfRandMax;
	ret.z = rand() - halfRandMax;
	return glm::normalize(ret);
}

glm::vec3 BGE::RotateVector(glm::vec3 v, glm::quat q)
{
	glm::quat qinv = glm::inverse(q);
	glm::quat w = glm::quat(0, v.x, v.y, v.z);
	w = q * w * qinv;

	return glm::vec3(w.x, w.y, w.z);
}

void BGE::SafeDelete(void ** p)
{
	void * pp = *p;
	if (pp != nullptr)
	{
		delete pp;
		*p = nullptr;
	}
}

glm::vec3 BGE::LeapToGlVec3(Leap::Vector v)
{
	return glm::vec3(v.x, v.y, v.z);
}

void BGE::CheckOverflow(int & x)
{
	if (x == -32768)
	{
		x = -x;
	}
}
