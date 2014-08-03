#pragma once
#include "XBoxController.h"
#include "GameComponent.h"
#include <OVR.h>
#include <OVR_CAPI_GL.h>
#include "FBO.h"

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
		bool Initialise();
		void DrawToRift();
		void Cleanup();
		void Connect();

		// Rift specific stuff!
		ovrHmd hmd;
		OVR::Sizei renderTargetSize;
		GLuint texture;
		GLuint frameBuffer;
		GLuint renderBuffer;
		ovrEyeRenderDesc eyeRenderDesc[2];
		ovrFovPort eyeFov[2];
		ovrRecti eyeRenderViewport[2];
		ovrGLTexture eyeTexture[2];
		void AccumulateInputs();

	};
}
