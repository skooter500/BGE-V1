#pragma once
#include "Steerable3DController.h"
#include "GameComponent.h"
#include "NuiApi.h"
#include "SteeringControler.h"



namespace BGE
{
	void CALLBACK StatusProc1( HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData);

	class KinectFlyingController:public GameComponent
	{
	public:
		KinectFlyingController(shared_ptr<Model> model);
		~KinectFlyingController(void);

		INuiSensor* m_pNuiSensor;

		bool connected;
		bool tracked;
		float footHeight;
		float scale;
		float myRoll;
		HRESULT CreateFirstConnected();
		void Update(float timeDelta);
		bool Initialise();
		void UpdateSkeleton(const NUI_SKELETON_DATA & skeleton);
		void SkeletonFrameReady(NUI_SKELETON_FRAME* pSkeletonFrame);
		float timeDelta;
		shared_ptr<Steerable3DController> steerable;
		HANDLE m_hNextSkeletonEvent;

		void SetStatusMessage(std::string message);
		glm::vec3 leftHandPos;
		glm::vec3 rightHandPos;
		glm::vec3 shoulderPos;
	};
}

