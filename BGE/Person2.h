#pragma once
#include "GameComponent.h"
#include "PhysicsController.h"
#include <Kinect.h>
#include <string>
#include <map>

using namespace std;

namespace BGE
{
	class Person2 :
		public GameComponent
	{
	private:
		/*
		void UpdateBone( const NUI_SKELETON_DATA & skeleton, NUI_SKELETON_POSITION_INDEX jointFrom, NUI_SKELETON_POSITION_INDEX jointTo);
		
		void UpdateHand( const NUI_SKELETON_DATA & skeleton, NUI_SKELETON_POSITION_INDEX jointFrom, NUI_SKELETON_POSITION_INDEX jointTo, int handIndex);
		*/
		map<string, std::shared_ptr<PhysicsController>> boneComponents;
		bool connected;
		bool tracked;
		float footHeight;
		float scale;

		// Kinect STuff
		// Current Kinect
		IKinectSensor*          m_pKinectSensor;
		ICoordinateMapper*      m_pCoordinateMapper;
		HRESULT                 InitializeDefaultSensor();
		void                    ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies);

		// Body reader
		IBodyFrameReader*       m_pBodyFrameReader;

	public:
		Person2(void);
		~Person2(void);

		void Update(float timeDelta);
		bool Initialise();
		bool headCamera;
		//Hand hands[2];

		void SetStatusMessage(std::string message);
		void UpdateBone(const Joint* pJoints, JointType joint0, JointType joint1);
		void UpdateHead(const Joint* pJoints, JointType joint0);

	};
}

