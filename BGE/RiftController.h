#pragma once
#include "XBoxController.h"
#include "GameComponent.h"
#include <OVR.h>
#include "FBO.h"

using namespace OVR;

namespace BGE
{
	class RiftController :
		public GameComponent
	{
	public:
		RiftController(void);
		~RiftController(void);

		std::shared_ptr<BGE::XBoxController> xboxController;

		void Update(float timeDelta);
		bool RiftController::Initialise();

		// Rift specific stuff!

		bool       SensorActive() const { return m_pSensor != NULL; }
		bool       GetStereoMode() const { return m_SConfig.GetStereoMode() == OVR::Util::Render::Stereo_LeftRight_Multipass; }
		const OVR::HMDInfo& GetHMD() const { return m_HMDInfo; }

		int GetRenderBufferWidth() const { return m_fboWidth; }
		int GetRenderBufferHeight() const { return m_fboHeight; }

		void InitOVR();
		void DestroyOVR();
		void CreateShaders();
		void CreateRenderBuffer();
		void BindRenderBuffer();
		void UnBindRenderBuffer();

		void PresentFbo();
		void PresentFbo_NoDistortion();
		void PresentFbo_PostProcessDistortion(const OVR::Util::Render::StereoEyeParams& eyeParams);

		void AccumulateInputs();

		OVR::Ptr<OVR::DeviceManager>  m_pManager;
		OVR::Ptr<OVR::HMDDevice> m_pHMD;
		OVR::Ptr<OVR::SensorDevice> m_pSensor;
		OVR::SensorFusion m_SFusion;
		OVR::HMDInfo m_HMDInfo;
		OVR::Matrix4f View;

		/// Stereo viewing parameters
		enum PostProcessType
		{
			PostProcess_None,
			PostProcess_Distortion 
		};

		OVR::Util::Render::StereoConfig   m_SConfig;
		PostProcessType m_PostProcess;

		enum DisplayMode
		{
			SingleEye,
			Stereo,
			StereoWithDistortion
		};

		void SetDisplayMode(DisplayMode);

		/// Render buffer for OVR distortion correction shader
		FBO m_renderBuffer;
		int m_fboWidth;
		int m_fboHeight;

		GLuint m_progRiftDistortion;
		GLuint m_progPresFbo;

		int m_windowWidth;
		int m_windowHeight;
	};
}
