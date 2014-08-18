#pragma once
#include "GameComponent.h"
#include "PhysicsController.h"
#include <Kinect.h>
#include <string>
#include <map>

using namespace std;

namespace BGE
{

	struct KinectHand
	{
		glm::vec3 pos;
		glm::vec3 look;
		HandState state;
	};

	class Person2 :
		public GameComponent
	{
	private:

		map<string, std::shared_ptr<PhysicsController>> boneComponents;
		bool connected;
		bool tracked;
		float footHeight;
		float scale;

		// Kinect STuff
		// Current Kinect
		IKinectSensor * m_pKinectSensor;
		ICoordinateMapper * m_pCoordinateMapper;
		HRESULT InitializeDefaultSensor();
		void ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies);
		
		// Body reader
		IBodyFrameReader*       m_pBodyFrameReader;

		void UpdateHands(IBody * pBody, Joint * joints);
		glm::vec3 Person2::Scale(glm::vec3 point);
	public:
		Person2(void);
		~Person2(void);

		void Update(float timeDelta);
		bool Initialise();
		bool headCamera;
		KinectHand hands[2];

		void SetStatusMessage(std::string message);
		void UpdateBone(const Joint* pJoints, JointType joint0, JointType joint1);
		void UpdateHead(const Joint* pJoints, JointType joint0);

	};
}

