#pragma once
#include "GameComponent.h"
#include <OVR.h>

using namespace OVR;

namespace BGE
{
	class RiftController :
		public GameComponent
	{
	public:
		RiftController(void);
		~RiftController(void);

		void Update(float timeDelta);
		static glm::quat OVRToGLQuat(OVR::Quatf q);

		//// Rift Stuff
		//Ptr<DeviceManager>  pManager;
		//Ptr<SensorDevice>   pSensor;
		//Ptr<HMDDevice>      pHMD;
		//SensorFusion        SFusion;
		//OVR::HMDInfo        HMDInfo;
	};
}
