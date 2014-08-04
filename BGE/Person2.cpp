#include "Person2.h"
#include <sstream>
#include "Box.h"
#include "PhysicsGame1.h"
#include "Cylinder.h"
#include "Utils.h"
#include "PhysicsController.h"
#include "KinematicMotionState.h"
#include <btBulletDynamicsCommon.h>

using namespace BGE;
using namespace std;

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

glm::vec3 KinectToGLVector(CameraSpacePoint v, bool flip)
{
	return glm::vec3(v.X, v.Y, v.Z);
}

Person2::Person2(void):GameComponent(true)
{
	connected = false;
	tracked = false;
	headCamera = false;
	scale = 20.0f;
	footHeight = 0.0f;
}


Person2::~Person2(void)
{
	// close the Kinect Sensor
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);
}

bool Person2::Initialise()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		connected = true;

		SafeRelease(pBodyFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		SetStatusMessage("No ready Kinect found!");
		connected = false;
		return false;
	}
	return GameComponent::Initialise();
}

void Person2::UpdateBone(const Joint* pJoints, JointType joint0, JointType joint1)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;
	TrackingState joint1State = pJoints[joint1].TrackingState;

	// If we can't find either of these joints, exit
	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
	{
		return;
	}

	// Don't draw if both points are inferred
	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
	{
		return;
	}

	glm::vec3 start = KinectToGLVector(pJoints[joint0].Position, !headCamera);
	glm::vec3 end = KinectToGLVector(pJoints[joint1].Position, !headCamera);
	
	start.y -= footHeight;
	end.y -= footHeight;

	start *= scale;
	end *= scale;
	
	glm::vec3 boneVector = end - start;
	float boneLength = glm::length(boneVector);
	glm::vec3 centrePos = start + ((boneVector) / 2.0f);
	
	boneVector = glm::normalize(boneVector);
	glm::vec3 axis = glm::cross(Transform::basisUp, boneVector);
	axis = glm::normalize(axis);
	float theta = (float) glm::acos<float>(glm::dot<float>(Transform::basisUp, boneVector));
	glm::quat q = glm::angleAxis(glm::degrees(theta), axis);

	stringstream ss;
	ss << joint0 << "," << joint1;
	string boneKey = ss.str();

	map<string, shared_ptr<PhysicsController>>::iterator it = boneComponents.find(boneKey);
	shared_ptr<PhysicsController> cylController;
	PhysicsGame1 * game = (PhysicsGame1 *) Game::Instance();
	if (it == boneComponents.end())
	{
		cylController = game->physicsFactory->CreateCylinder(0.5f, boneLength, centrePos, transform->orientation);
		cylController->rigidBody->setCollisionFlags(cylController->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		cylController->rigidBody->setActivationState(DISABLE_DEACTIVATION);
		cylController->rigidBody->setMotionState(new KinematicMotionState(cylController->parent));
		cylController->tag = "Person2Bone";
		boneComponents[boneKey] = cylController;
	}
	else
	{
		cylController = it->second;
	}
	cylController->transform->position = this->transform->position + centrePos;
	cylController->parent->transform->orientation = q;
}

void Person2::UpdateHead(const Joint* pJoints, JointType joint0)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;

	// If we can't find either of these joints, exit
	if (joint0State == TrackingState_NotTracked)
	{
		return;
	}

	glm::vec3 start = KinectToGLVector(pJoints[joint0].Position, !headCamera);

	start.y -= footHeight;
	
	start *= scale;
	
	glm::quat q = glm::angleAxis(glm::degrees(glm::pi<float>()), glm::vec3(1,0,0));

	stringstream ss;
	ss << joint0;
	string boneKey = ss.str();

	PhysicsGame1 * game = (PhysicsGame1 *) Game::Instance();

	map<string, shared_ptr<PhysicsController>>::iterator it = boneComponents.find(boneKey);
	shared_ptr<PhysicsController> boxController;
	if (it == boneComponents.end())
	{
		boxController = game->physicsFactory->CreateBox(6.0f, 6.0f, 0.5f, start, transform->orientation);
		boxController->rigidBody->setCollisionFlags(boxController->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		boxController->rigidBody->setActivationState(DISABLE_DEACTIVATION);
		boxController->rigidBody->setMotionState(new KinematicMotionState(boxController->parent));
		boxController->tag = "Person2Head";

		boneComponents[boneKey] = boxController;
	}
	else
	{
		boxController = it->second;
	}

	if (headCamera)
	{
		game->camera->transform->position = start + glm::vec3(0, scale * 0.2f, 0);
		boxController->transform->position = glm::vec3(100, -100, 100);
	}
	else
	{
		boxController->transform->position = this->transform->position + start;
	}
	boxController->parent->transform->orientation = q;
	
}

void Person2::Update(float timeDelta)
{
	IBodyFrame* pBodyFrame = NULL;

	if (!m_pBodyFrameReader)
	{
		SetStatusMessage("Kinect 2 body frame reader is null");
		return;
	}

	HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

	if (SUCCEEDED(hr))
	{
		tracked = true;
		INT64 nTime = 0;
		hr = pBodyFrame->get_RelativeTime(&nTime);
		IBody* ppBodies[BODY_COUNT] = { 0 };
		
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			ProcessBody(nTime, BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}
	SafeRelease(pBodyFrame);

	if (connected)
	{
		SetStatusMessage("Kinect is connected");
		SetStatusMessage("Press C to toggle the head camera");
		if (tracked)
		{
			SetStatusMessage("Kinect 2 is tracking");
		}
		else
		{
			SetStatusMessage("Kinect 2 is not tracking");
		}		
	}
	else
	{
		SetStatusMessage("Kinect 2 is not connected");
	}

	GameComponent::Update(timeDelta);
}

void Person2::ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies)
{
	
	Game::Instance()->PrintFloat("Body count: ", nBodyCount);
	if (m_pCoordinateMapper && (nBodyCount > 0))
	{
		for (int i = 0; i < nBodyCount; ++i)
		{
			IBody* pBody = ppBodies[i];
			if (pBody)
			{			
				BOOLEAN bTracked = false;
				HRESULT hr = pBody->get_IsTracked(&bTracked);

				if (SUCCEEDED(hr) && bTracked)
				{
					Joint joints[JointType_Count];
					HandState leftHandState = HandState_Unknown;
					HandState rightHandState = HandState_Unknown;

					pBody->get_HandLeftState(&leftHandState);
					pBody->get_HandRightState(&rightHandState);

					hr = pBody->GetJoints(_countof(joints), joints);
					if (SUCCEEDED(hr))
					{
						for (int j = 0; j < _countof(joints); ++j)
						{
							//joints[j].
						}

						if (footHeight == 0.0f)
						{
							footHeight = glm::min<float>(joints[JointType_FootLeft].Position.Y, joints[JointType_FootLeft].Position.Y);
						}

						UpdateBone(joints, JointType_Head, JointType_Neck);
						UpdateBone(joints, JointType_Neck, JointType_SpineShoulder);
						UpdateBone(joints, JointType_SpineShoulder, JointType_SpineMid);
						UpdateBone(joints, JointType_SpineMid, JointType_SpineBase);
						UpdateBone(joints, JointType_SpineShoulder, JointType_ShoulderRight);
						UpdateBone(joints, JointType_SpineShoulder, JointType_ShoulderLeft);
						UpdateBone(joints, JointType_SpineBase, JointType_HipRight);
						UpdateBone(joints, JointType_SpineBase, JointType_HipLeft);

						// Right Arm    
						UpdateBone(joints, JointType_ShoulderRight, JointType_ElbowRight);
						UpdateBone(joints, JointType_ElbowRight, JointType_WristRight);
						UpdateBone(joints, JointType_WristRight, JointType_HandRight);
						UpdateBone(joints, JointType_HandRight, JointType_HandTipRight);
						UpdateBone(joints, JointType_WristRight, JointType_ThumbRight);

						// Left Arm
						UpdateBone(joints, JointType_ShoulderLeft, JointType_ElbowLeft);
						UpdateBone(joints, JointType_ElbowLeft, JointType_WristLeft);
						UpdateBone(joints, JointType_WristLeft, JointType_HandLeft);
						UpdateBone(joints, JointType_HandLeft, JointType_HandTipLeft);
						UpdateBone(joints, JointType_WristLeft, JointType_ThumbLeft);

						// Right Leg
						UpdateBone(joints, JointType_HipRight, JointType_KneeRight);
						UpdateBone(joints, JointType_KneeRight, JointType_AnkleRight);
						UpdateBone(joints, JointType_AnkleRight, JointType_FootRight);

						// Left Leg
						UpdateBone(joints, JointType_HipLeft, JointType_KneeLeft);
						UpdateBone(joints, JointType_KneeLeft, JointType_AnkleLeft);
						UpdateBone(joints, JointType_AnkleLeft, JointType_FootLeft);

						UpdateHead(joints, JointType_Head);

						//DrawBody(joints, jointPoints);



						//DrawHand(leftHandState, jointPoints[JointType_HandLeft]);
						//DrawHand(rightHandState, jointPoints[JointType_HandRight]);
					}
				}
			}
		}
	}
}

void Person2::SetStatusMessage( std::string message )
{
	Game::Instance()->PrintText(message);
}
