#include "RiftController.h"
#include "Game.h"
#include "Content.h"
#include "GameComponent.h"
#include "ShaderFunctions.h"
#include "Utils.h"
#include "SDL_syswm.h"
#include <OVR_Math.h>

using namespace BGE;
using namespace std;

void RiftController::AccumulateInputs()
{
	ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds());
	if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
	{
		OVR::Posef pose = ts.HeadPose.ThePose;
		glm::quat headOrientation = OVRToGLQuat(pose.Rotation);
		transform->position = xboxController->transform->position + (OVRToGLVector(pose.Translation) * 20.0f);
		//cout << transform->position.x << " " << transform->position.y << " " << transform->position.z << endl;
		transform->orientation = xboxController->transform->orientation * headOrientation;
		// Now update the XBOX Controller Look vectors
		glm::quat tempQ = xboxController->transform->orientation;
		xboxController->transform->orientation = transform->orientation;
		xboxController->transform->RecalculateVectors();
		xboxController->transform->orientation = tempQ;
	}
}

RiftController::RiftController()
{
	tag = "RiftController";
}


RiftController::~RiftController(void)
{
	
}

void RiftController::DrawToRift()
{
	ovrFrameTiming frameTiming = ovrHmd_BeginFrame(hmd, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(true);
	ovrPosef eyeRenderPose[2];
	static float    BodyYaw(0);

	static glm::mat4 cameraView = Game::Instance()->camera->view;
	for (int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
	{
		ovrEyeType eye = hmd->EyeRenderOrder[eyeIndex];
		eyeRenderPose[eye] = ovrHmd_GetEyePose(hmd, eye);

		// Get view and projection matrices
		// Slightly modified from the SDK example for OpenGL
		OVR::Matrix4f rollPitchYaw = GLToOVRMat4(glm::mat4_cast(xboxController->transform->orientation));
		OVR::Matrix4f finalRollPitchYaw = rollPitchYaw * OVR::Matrix4f(eyeRenderPose[eye].Orientation);
		OVR::Vector3f finalUp = finalRollPitchYaw.Transform(OVR::Vector3f(0, 1, 0));
		OVR::Vector3f finalForward = finalRollPitchYaw.Transform(OVR::Vector3f(0, 0, -1));
		OVR::Vector3f shiftedEyePos = OVR::Vector3f(GLToOVRVector(transform->position)) + rollPitchYaw.Transform(eyeRenderPose[eye].Position);
		OVR::Matrix4f ovrView = OVR::Matrix4f::Translation(eyeRenderDesc[eye].ViewAdjust) * OVR::Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
		OVR::Matrix4f ovrProjection = ovrMatrix4f_Projection(eyeRenderDesc[eye].Fov, 0.01f, 10000.0f, true);

		glViewport(eyeRenderViewport[eye].Pos.x, eyeRenderViewport[eye].Pos.y, eyeRenderViewport[eye].Size.w, eyeRenderViewport[eye].Size.h);
		Game::Instance()->camera->view = OVRToGLMat4(ovrView);
		Game::Instance()->camera->projection = OVRToGLMat4(ovrProjection);
		// Draw the scene		
		Game::Instance()->GameComponent::Draw();
		LineDrawer::Instance()->Draw();				
	}	
	Game::Instance()->camera->view = cameraView;
	ovrHmd_EndFrame(hmd, eyeRenderPose, &eyeTexture[0].Texture);
}

void RiftController::Cleanup()

{
	ovrHmd_Destroy(hmd);
}


void RiftController::Update(float timeDelta)
{
	AccumulateInputs();
	ovrHSWDisplayState hswDisplayState;
	
	// Dismiss the health and safety warning
	ovrHmd_GetHSWDisplayState(hmd, &hswDisplayState);
	if (hswDisplayState.Displayed)
	{
		ovrHmd_DismissHSWDisplay(hmd);
	}
	GameComponent::Update(timeDelta);
}

void RiftController::Connect()
{
	ovr_Initialize();
	hmd = ovrHmd_Create(0);
	if (hmd == nullptr)
	{
		hmd = ovrHmd_CreateDebug(ovrHmd_DK1);
	}
	// Start the sensor which provides the Rift’s pose and motion.
	ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
		ovrTrackingCap_MagYawCorrection |
		ovrTrackingCap_Position, 0);
}

bool RiftController::Initialise()
{	
	// Configure OpenGL rendering
	OVR::Sizei recommendedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, hmd->DefaultEyeFov[0], 1.0f);
	OVR::Sizei recommendedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, hmd->DefaultEyeFov[1], 1.0f);
	renderTargetSize.w = recommendedTex0Size.w + recommendedTex1Size.w;
	renderTargetSize.h = max(recommendedTex0Size.h, recommendedTex1Size.h);

	glGenFramebuffers(1, &frameBuffer);

	glGenTextures(1, &texture);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderTargetSize.w, renderTargetSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, renderTargetSize.w, renderTargetSize.h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glDeleteFramebuffers(1, &frameBuffer);
		glDeleteTextures(1, &texture);
		glDeleteRenderbuffers(1, &renderBuffer);

		SDL_GL_DeleteContext(Game::Instance()->context);
		SDL_DestroyWindow(Game::Instance()->window);
		ovrHmd_Destroy(hmd);
		ovr_Shutdown();
		SDL_Quit();
		throw BGE::Exception("Framebuffer error");
	}

	eyeFov[0] = hmd->DefaultEyeFov[0];
	eyeFov[1] = hmd->DefaultEyeFov[1];

	eyeRenderViewport[0].Pos = OVR::Vector2i(0, 0);
	eyeRenderViewport[0].Size = OVR::Sizei(renderTargetSize.w / 2, renderTargetSize.h);
	eyeRenderViewport[1].Pos = OVR::Vector2i((renderTargetSize.w + 1) / 2, 0);
	eyeRenderViewport[1].Size = eyeRenderViewport[0].Size;

	eyeTexture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
	eyeTexture[0].OGL.Header.TextureSize = renderTargetSize;
	eyeTexture[0].OGL.Header.RenderViewport = eyeRenderViewport[0];
	eyeTexture[0].OGL.TexId = texture;

	eyeTexture[1] = eyeTexture[0];
	eyeTexture[1].OGL.Header.RenderViewport = eyeRenderViewport[1];

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(Game::Instance()->window, &info);

	ovrGLConfig cfg;
	cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
	cfg.OGL.Header.RTSize = OVR::Sizei(hmd->Resolution.w, hmd->Resolution.h);
	cfg.OGL.Header.Multisample = 1;
#ifdef _WIN32
	if (!(hmd->HmdCaps & ovrHmdCap_ExtendDesktop))
		ovrHmd_AttachToWindow(hmd, info.info.win.window, NULL, NULL);

	cfg.OGL.Window = info.info.win.window;
	cfg.OGL.DC = NULL;
#endif

	ovrHmd_ConfigureRendering(hmd, &cfg.Config, ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette | ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive, eyeFov, eyeRenderDesc);
	ovrHmd_SetEnabledCaps(hmd, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction); 
	//ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	xboxController = make_shared<XBoxController>(true);
	xboxController->disablePitch = true;
	xboxController->transform->position = transform->position;
	Attach(xboxController);
	return GameComponent::Initialise();
}

