#include "RiftController.h"
#include "Game.h"
#include "Content.h"
#include "GameComponent.h"
#include "OVR_Shaders.h"
#include "ShaderFunctions.h"
#include "Utils.h"

using namespace BGE;
using namespace OVR;
using namespace std;

void RiftController::AccumulateInputs()
{
	if (SensorActive())
	{
		OVR::Quatf hmdOrient = m_SFusion.GetOrientation();
		glm::quat headOrientation = OVRToGLQuat(hmdOrient);
		// Combine the orientation of the head with the yaw from the XBOX controller...
		orientation = xboxController->orientation * headOrientation;
		//position = xboxController->position;
		// Now update the XBOX Controller Look vectors
		glm::quat tempQ = xboxController->orientation;
		xboxController->orientation = orientation;
		xboxController->RecalculateVectors();
		xboxController->orientation = tempQ;
	}
}

RiftController::RiftController(): m_pManager(NULL)
	, m_pHMD(NULL)
	, m_pSensor(NULL)
	, m_SFusion()
	, m_HMDInfo()
	, m_SConfig()
	, m_PostProcess(PostProcess_Distortion)
	, m_fboWidth(0)
	, m_fboHeight(0)
	, m_progRiftDistortion(0)
	, m_progPresFbo(0)
	, m_windowWidth(0)
	, m_windowHeight(0)
{
	worldMode = world_modes::to_parent;
	xboxController = make_shared<XBoxController>();
	xboxController->worldMode = GameComponent::from_self;
	xboxController->disablePitch = true;
	Attach(xboxController);
}


RiftController::~RiftController(void)
{
	DestroyOVR();
}



void RiftController::Update(float timeDelta)
{
	AccumulateInputs();
	GameComponent::Update(timeDelta);
}

bool RiftController::Initialise()
{
	InitOVR();
	SetDisplayMode(StereoWithDistortion);
	CreateShaders();
	CreateRenderBuffer();
	xboxController->position = position;
	return GameComponent::Initialise();
}


/// Once source is obtained from either file or hard-coded map, compile the
/// shader, release the string memory and return the ID.
GLuint makeShaderFromSource(const char* source, const unsigned long Type)
{
	if (source == NULL)
		return 0;
	GLint length = strlen(source);

	GLuint shaderId = glCreateShader(Type);
	glShaderSource(shaderId, 1, &source, &length);
	glCompileShader(shaderId);

	return shaderId;
}

/// Create a shader program from the OVR sources.
GLuint CreateOVRShaders()
{
	GLuint vertSrc = makeShaderFromSource(PostProcessVertexShaderSrc, GL_VERTEX_SHADER);
	GLuint fragSrc = makeShaderFromSource(PostProcessFragShaderSrc, GL_FRAGMENT_SHADER);

	printShaderInfoLog(vertSrc);
	printShaderInfoLog(fragSrc);

	GLuint program = glCreateProgram();

	glCompileShader(vertSrc);
	glCompileShader(fragSrc);

	glAttachShader(program, vertSrc);
	glAttachShader(program, fragSrc);

	// Will be deleted when program is.
	glDeleteShader(vertSrc);
	glDeleteShader(fragSrc);

	glLinkProgram(program);
	printProgramInfoLog(program);

	glUseProgram(0);
	return program;
}


void RiftController::DestroyOVR()
{
	/// Clear these before calling Destroy.
	m_pSensor.Clear();
	m_pManager.Clear();
	m_pHMD.Clear();
	// No OVR functions involving memory are allowed after this.
	OVR::System::Destroy();
}

/// We need an active GL context for this
void RiftController::CreateShaders()
{
	m_progPresFbo = Content::LoadShaderPair("presentFbo");
	m_progRiftDistortion = CreateOVRShaders();
}

void RiftController::CreateRenderBuffer()
{
	allocateFBO(m_renderBuffer, m_fboWidth, m_fboHeight);
}

void RiftController::BindRenderBuffer()
{
	bindFBO(m_renderBuffer);
}

void RiftController::UnBindRenderBuffer()
{
	unbindFBO();
}

void RiftController::PresentFbo_NoDistortion()
{
	glUseProgram(m_progPresFbo);
	{
		OVR::Matrix4f ortho = OVR::Matrix4f::Ortho2D((float)m_fboWidth, (float)m_fboHeight);
		glUniformMatrix4fv(getUniLoc(m_progPresFbo, "prmtx"), 1, false, &ortho.Transposed().M[0][0]);

		const float verts[] = {
			0                ,  0,
			(float)m_fboWidth,  0,
			(float)m_fboWidth, (float)m_fboHeight,
			0                , (float)m_fboHeight,
		};
		const float texs[] = {
			0,1,
			1,1,
			1,0,
			0,0,
		};
		const unsigned int tris[] = {
			0,1,2, 0,3,2, // ccw
		};

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, texs);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_renderBuffer.tex);
		glUniform1i(getUniLoc(m_progPresFbo, "fboTex"), 0);

		glDrawElements(GL_TRIANGLES,
			6,
			GL_UNSIGNED_INT,
			&tris[0]);
	}
	glUseProgram(0);
}


///@todo It would be pretty cool here to get all these parameters hooked up properly to some set
/// of controls and config files for custimizing...
void RiftController::PresentFbo_PostProcessDistortion(const OVR::Util::Render::StereoEyeParams& eyeParams)
{
	const OVR::Util::Render::DistortionConfig*  pDistortion = eyeParams.pDistortion;
	if (pDistortion == NULL)
		return;

	glUseProgram(m_progRiftDistortion);
	{
		/// Set uniforms
		OVR::Matrix4f ident;
		glUniformMatrix4fv(getUniLoc(m_progRiftDistortion, "View"), 1, false, &ident.Transposed().M[0][0]);
		glUniformMatrix4fv(getUniLoc(m_progRiftDistortion, "Texm"), 1, false, &ident.Transposed().M[0][0]);

		/// this value ripped from the TinyRoom demo at runtime
		const float lensOff = 0.287994f - 0.25f;

		/// The left screen is centered at (0.25, 0.5)
		glUniform2f(getUniLoc(m_progRiftDistortion, "LensCenter"),
			//pDistortion->XCenterOffset,
			//pDistortion->YCenterOffset
			0.25f + lensOff, 0.5f);

		glUniform2f(getUniLoc(m_progRiftDistortion, "ScreenCenter"),
			0.25f, 0.5f);

		/// The right screen is centered at (0.75, 0.5)
		if (eyeParams.Eye == OVR::Util::Render::StereoEye_Right)
		{
			glUniform2f(getUniLoc(m_progRiftDistortion, "ScreenCenter"),
				0.75f, 0.5f);

			glUniform2f(getUniLoc(m_progRiftDistortion, "LensCenter"),
				0.75f - lensOff, 0.5f);
		}

		glUniform2f(getUniLoc(m_progRiftDistortion, "Scale"),
			//pDistortion->Scale,
			//pDistortion->Scale
			0.145806f,  0.233290f);

		glUniform2f(getUniLoc(m_progRiftDistortion, "ScaleIn"),
			//pDistortion->Scale,
			//pDistortion->Scale
			4.0f, 2.5f);

		glUniform4f(getUniLoc(m_progRiftDistortion, "HmdWarpParam"),
			pDistortion->K[0],
			pDistortion->K[1],
			pDistortion->K[2],
			pDistortion->K[3]
		);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_renderBuffer.tex);
		glUniform1i(getUniLoc(m_progRiftDistortion, "Texture0"), 0);

		float verts[] = { /// Left eye coords
			-1.0f, -1.0f,
			0.0f, -1.0f,
			0.0f,  1.0f,
			-1.0f,  1.0f,
		};
		float texs[] = {
			0.0f, 1.0f,
			0.5f, 1.0f,
			0.5f, 0.0f,
			0.0f, 0.0f,
		};

		/// Adjust coords for right eye
		if (eyeParams.Eye == OVR::Util::Render::StereoEye_Right)
		{
			verts[2*0  ] += 1.0f;
			verts[2*1  ] += 1.0f;
			verts[2*2  ] += 1.0f;
			verts[2*3  ] += 1.0f;

			texs[2*0] += 0.5f;
			texs[2*1] += 0.5f;
			texs[2*2] += 0.5f;
			texs[2*3] += 0.5f;
		}


		// BD Fixed the winding order of these vertices so backface culling works...
		const unsigned int tris[] = {
			0,1,2,  0,2,3
		};

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, texs);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glDrawElements(GL_TRIANGLES,
			6,
			GL_UNSIGNED_INT,
			&tris[0]);
	}
	glUseProgram(0);
}

void RiftController::PresentFbo()
{
	if (m_PostProcess == PostProcess_Distortion)
	{
		const OVR::Util::Render::StereoEyeParams& LeyeParams =
			m_SConfig.GetEyeRenderParams(OVR::Util::Render::StereoEye_Left);
		PresentFbo_PostProcessDistortion(LeyeParams);

		const OVR::Util::Render::StereoEyeParams& ReyeParams =
			m_SConfig.GetEyeRenderParams(OVR::Util::Render::StereoEye_Right);
		PresentFbo_PostProcessDistortion(ReyeParams);
	}
	else
	{
		PresentFbo_NoDistortion();
	}
}


/// Init OVR
void RiftController::InitOVR()
{
	OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
	m_pManager = *OVR::DeviceManager::Create();
	m_pHMD  = *m_pManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
	if (m_pHMD == NULL)
	{
		/// If the HMD is turned off or not present, fill in some halfway sensible
		/// default values here so we can at least see some rendering.
		///@todo Grab these values from the HMD setup, see how we did on guesing
		OVR::HMDInfo& hmd = m_HMDInfo;
		hmd.DesktopX = 0;
		hmd.DesktopY = 0;
		hmd.HResolution = 1280;
		hmd.VResolution = 800;

		hmd.HScreenSize = 0.09f;
		hmd.VScreenSize = 0.06f;
		hmd.VScreenCenter = 0.02f;

		hmd.DistortionK[0] = 1.0f;
		hmd.DistortionK[1] = 0.5f;
		hmd.DistortionK[2] = 0.25f;
		hmd.DistortionK[3] = 0.0f;

		hmd.EyeToScreenDistance = 0.01f;
		hmd.InterpupillaryDistance = 0.064f;
		hmd.LensSeparationDistance = 0.07f;
	}
	else
	{
		m_pSensor = *m_pHMD->GetSensor();

		// This will initialize HMDInfo with information about configured IPD,
		// screen size and other variables needed for correct projection.
		// We pass HMD DisplayDeviceName into the renderer to select the
		// correct monitor in full-screen mode.
		if (m_pHMD->GetDeviceInfo(&m_HMDInfo))
		{
			m_SConfig.SetHMDInfo(m_HMDInfo);
		}
		if (m_pSensor)
		{

			m_SFusion.AttachToSensor(m_pSensor);
		}
	}

	if (m_HMDInfo.HResolution > 0)
	{
		m_windowWidth  = m_HMDInfo.HResolution;
		m_windowHeight = m_HMDInfo.VResolution;
	}

	// *** Configure Stereo settings.
	m_SConfig.SetFullViewport(OVR::Util::Render::Viewport(0,0, m_windowWidth, m_windowHeight));
	m_SConfig.SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);

	// Configure proper Distortion Fit.
	// For 7" screen, fit to touch left side of the view, leaving a bit of invisible
	// screen on the top (saves on rendering cost).
	// For smaller screens (5.5"), fit to the top.
	if (m_HMDInfo.HScreenSize > 0.0f)
	{
		if (m_HMDInfo.HScreenSize > 0.140f) // 7"
			m_SConfig.SetDistortionFitPointVP(-1.0f, 0.0f);
		else
			m_SConfig.SetDistortionFitPointVP(0.0f, 1.0f);
	}
	m_SConfig.Set2DAreaFov(OVR::DegreeToRad(85.0f));

	/// For OVR distortion correction shader
	const float renderBufferScaleIncrease = m_SConfig.GetDistortionScale();
	m_fboWidth = (int)((renderBufferScaleIncrease) * (float)m_windowWidth );
	m_fboHeight = (int)((renderBufferScaleIncrease) * (float)m_windowHeight );
}


void RiftController::SetDisplayMode(DisplayMode mode)
{
	switch(mode)
	{
	default:
	case SingleEye:
		m_SConfig.SetStereoMode(OVR::Util::Render::Stereo_None);
		m_PostProcess = PostProcess_None;
		break;

	case Stereo:
		m_SConfig.SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);
		m_PostProcess = PostProcess_None;
		break;

	case StereoWithDistortion:
		m_SConfig.SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);
		m_PostProcess = PostProcess_Distortion;
		break;
	}
}
