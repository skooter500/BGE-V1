#include "RiftController.h"
#include "Game.h"
#include "Content.h"
#include "GameComponent.h"
#include "ShaderFunctions.h"
#include "Utils.h"
#include "SDL_syswm.h"

using namespace BGE;
using namespace std;

void RiftController::AccumulateInputs()
{
	ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds());
	if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
	{
	}

	//if (SensorActive())
	//{
	//	OVR::Quatf hmdOrient = m_SFusion.GetOrientation();
	//	glm::quat headOrientation = OVRToGLQuat(hmdOrient);
	//	// Combine the orientation of the head with the yaw from the XBOX controller...
	//	transform->orientation = xboxController->transform->orientation * headOrientation;
	//	//position = xboxController->position;
	//	// Now update the XBOX Controller Look vectors
	//	glm::quat tempQ = xboxController->transform->orientation;
	//	xboxController->transform->orientation = transform->orientation;
	//	xboxController->transform->RecalculateVectors();
	//	xboxController->transform->orientation = tempQ;
	//}
}

RiftController::RiftController()
{
	xboxController = make_shared<XBoxController>();
	xboxController->disablePitch = true;
	Attach(xboxController);
}


RiftController::~RiftController(void)
{
	
}

void RiftController::Draw()
{
	OVR::Sizei recommendedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, hmd->DefaultEyeFov[0], 1.0f);
	OVR::Sizei recommendedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, hmd->DefaultEyeFov[1], 1.0f);
	renderTargetSize.w = recommendedTex0Size.w + recommendedTex1Size.w;
	renderTargetSize.h = max(recommendedTex0Size.h, recommendedTex1Size.h);
	eyeFov[0] = hmd->DefaultEyeFov[0];
	eyeFov[1] = hmd->DefaultEyeFov[1];
	ovrRecti eyeRenderViewport[2];
	eyeRenderViewport[0].Pos = OVR::Vector2i(0, 0);
	eyeRenderViewport[0].Size = OVR::Sizei(renderTargetSize.w / 2, renderTargetSize.h);
	eyeRenderViewport[1].Pos = OVR::Vector2i((renderTargetSize.w + 1) / 2, 0);
	eyeRenderViewport[1].Size = eyeRenderViewport[0].Size;

	ovrGLTexture eyeTexture[2];
	eyeTexture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
	eyeTexture[0].OGL.Header.TextureSize = renderTargetSize;
	eyeTexture[0].OGL.Header.RenderViewport = eyeRenderViewport[0];
	eyeTexture[0].OGL.TexId = texture;

	eyeTexture[1] = eyeTexture[0];
	eyeTexture[1].OGL.Header.RenderViewport = eyeRenderViewport[1];

	ovrFrameTiming frameTiming = ovrHmd_BeginFrame(hmd, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ovrPosef eyeRenderPose[2];

	for (int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
	{
		ovrEyeType eye = hmd->EyeRenderOrder[eyeIndex];
		eyeRenderPose[eye] = ovrHmd_GetEyePose(hmd, eye);

		OVR::Matrix4f ovrView = OVR::Matrix4f::Translation(eyeRenderDesc[eye].ViewAdjust) * OVR::Matrix4f(OVR::Quatf(eyeRenderPose[eye].Orientation).Inverted());
		OVR::Matrix4f ovrProjection = ovrMatrix4f_Projection(eyeRenderDesc[eye].Fov, 0.01f, 10000.0f, true);

		//Matrix4f MVPMatrix = Matrix4f(ovrMatrix4f_Projection(eyeRenderDesc[eye].Fov, 0.01f, 10000.0f, true)) * Matrix4f::Translation(eyeRenderDesc[eye].ViewAdjust) * Matrix4f(Quatf(eyeRenderPose[eye].Orientation).Inverted());
		glViewport(eyeRenderViewport[eye].Pos.x, eyeRenderViewport[eye].Pos.y, eyeRenderViewport[eye].Size.w, eyeRenderViewport[eye].Size.h);
		Game::Instance()->camera->view = OVRToGLMat4(ovrView);
		Game::Instance()->camera->projection = OVRToGLMat4(ovrProjection);
		// Draw the scene
		LineDrawer::Instance()->Draw();
		Game::Instance()->GameComponent::Draw();
	}

	glBindVertexArray(0);

	ovrHmd_EndFrame(hmd, eyeRenderPose, &eyeTexture[0].Texture);

}

void RiftController::Cleanup()
{
	ovrHmd_Destroy(hmd);
}


void RiftController::Update(float timeDelta)
{
	AccumulateInputs();
	GameComponent::Update(timeDelta);
}

bool RiftController::Initialise()
{
	ovr_Initialize();
	ovrHmd hmd = ovrHmd_Create(0);
	// Start the sensor which provides the Rift’s pose and motion.
	ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
		ovrTrackingCap_MagYawCorrection |
		ovrTrackingCap_Position, 0);

	// Configure OpenGL rendering
	ovrGLConfig cfg;
	cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
	cfg.OGL.Header.RTSize = OVR::Sizei(hmd->Resolution.w, hmd->Resolution.h);
	cfg.OGL.Header.Multisample = 1;
#ifdef _WIN32
	SDL_SysWMinfo info;

	SDL_VERSION(&info.version);

	SDL_GetWindowWMInfo(Game::Instance()->mainwindow, &info);
	if (!(hmd->HmdCaps & ovrHmdCap_ExtendDesktop))
		ovrHmd_AttachToWindow(hmd, info.info.win.window, NULL, NULL);

	cfg.OGL.Window = info.info.win.window;
	cfg.OGL.DC = NULL;
#endif
	ovrHmd_ConfigureRendering(hmd, &cfg.Config, ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette | ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive, eyeFov, eyeRenderDesc);

	glGenTextures(1, &texture);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderTargetSize.w, renderTargetSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	xboxController->transform->position = transform->position;
	return GameComponent::Initialise();
}

