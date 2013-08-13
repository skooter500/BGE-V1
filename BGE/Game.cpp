#include "Game.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <ctime>

#include <SDL_ttf.h>
#include "Content.h"
#include "FPSController.h"
#include "RiftController.h"
#include "Steerable3DController.h"
#include "OVRkill.h"

using namespace BGE;

BGE::Game * Game::instance = NULL;

OVRkill g_ok;

glm::vec3 BGE::RotateVector(glm::vec3 v, glm::quat q)
{
	glm::quat qinv = glm::inverse(q);
	glm::quat w = glm::quat(0, v.x, v.y, v.z);
	w = q * w * qinv;

	return glm::vec3(w.x, w.y, w.z);
}

void BGE::Log(string message)
{
	printf("%s\n", message.c_str());
}

Game::Game(void) {
	running = false;
	console = false;
	fullscreen = true;
	width = 1920;
	height = 1080;
	mainwindow = NULL;
	instance = this;
	srand(time(0));

	lastPrintPosition = glm::vec2(0,0);
	fontSize = 14;	

	renderToRift = true;
	worldMode = world_modes::from_self;

	camera = make_shared<Camera>();
	shared_ptr<GameComponent> controller = make_shared<FPSController>();
	controller->position = glm::vec3(0, 10, 10);
	//camera->AddChild(controller);
	
	shared_ptr<RiftController> riftController = make_shared<RiftController>();
	riftController->position = glm::vec3(0, 10, 10);
	camera->AddChild(riftController);
	AddChild(camera);

}

Game::~Game(void) {
}

shared_ptr<Ground> Game::GetGround()
{
	return ground;
}

void Game::DetectRift()
{
	//OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));

	//pManager = *DeviceManager::Create();

	//// We'll handle it's messages in this case.
	////pManager->SetMessageHandler(this);

 //   int         detectionResult = IDCONTINUE;
 //   const char* detectionMessage;

 //   pSensor.Clear();
 //   pHMD.Clear();
 //   //RenderParams.MonitorName.Clear();

 //   pHMD  = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();
 //   if (pHMD)
 //   {
 //       pSensor = *pHMD->GetSensor();

 //       // This will initialize HMDInfo with information about configured IPD,
 //       // screen size and other variables needed for correct projection.
 //       // We pass HMD DisplayDeviceName into the renderer to select the
 //       // correct monitor in full-screen mode.
 //       if (pHMD->GetDeviceInfo(&HMDInfo))
 //       {            
 //           //RenderParams.MonitorName = HMDInfo.DisplayDeviceName;
 //           //RenderParams.DisplayId = HMDInfo.DisplayId;
 //           //SConfig.SetHMDInfo(HMDInfo);
 //       }
 //   }
 //   else
 //   {            
 //       // If we didn't detect an HMD, try to create the sensor directly.
 //       // This is useful for debugging sensor interaction; it is not needed in
 //       // a shipping app.
 //       pSensor = *pManager->EnumerateDevices<SensorDevice>().CreateDevice();
 //   }


 //   // If there was a problem detecting the Rift, display appropriate message.
 //   detectionResult  = IDCONTINUE;        

 //   if (!pHMD && !pSensor)
 //       riftMessage = "Oculus Rift not detected.";
 //   else if (!pHMD)
 //       riftMessage = "Oculus Sensor detected; HMD Display not detected.";
 //   else if (!pSensor)
 //       riftMessage = "Oculus HMD Display detected; Sensor not detected.";
 //   else if (HMDInfo.DisplayDeviceName[0] == '\0')
 //       riftMessage = "Oculus Sensor detected; HMD display EDID not detected.";
 //   else
 //       riftMessage = "Oculus Rift detected!";

	//if (pSensor)
 //   {
 //       // We need to attach sensor to SensorFusion object for it to receive 
 //       // body frame messages and update orientation. SFusion.GetOrientation() 
 //       // is used in OnIdle() to orient the view.
 //       SFusion.AttachToSensor(pSensor);
 //       //SFusion.SetDelegateMessageHandler(this);
 //       SFusion.SetPredictionEnabled(true);
 //   }
}

bool Game::Initialise() {
	// Set up a console for debugging		
	if (console) 
	{
		AllocConsole();
		int fd = _open_osfhandle( (long)GetStdHandle( STD_OUTPUT_HANDLE ), 0); 
		FILE * fp = _fdopen( fd, "w" ); 
		*stdout = *fp; 
		setvbuf( stdout, NULL, _IONBF, 0 );
	}

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return false;
	}

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  4);
 
	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    /* Create our window centered at 512x512 resolution */
    mainwindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, flags);
	maincontext = SDL_GL_CreateContext(mainwindow);
	
    /* This makes our buffer swap syncronized with the monitor's vertical refresh */
    SDL_GL_SetSwapInterval(1);

	keyState = SDL_GetKeyboardState(NULL);
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		char msg[2048];
		sprintf(msg, "%s", glewGetErrorString(err));
		std::cerr << "Error:" << glewGetErrorString(err);
		return false;
	}
	
	glViewport(0, 0, width, height);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	SDL_GL_SetSwapInterval(1);

	if (TTF_Init() < 0)
	{
		throw BGE::Exception("Could not init TTF");
	}
	font = TTF_OpenFont("Content/arial.ttf",fontSize); // Open a font & set the font size

	//DetectRift();

	g_ok.InitOVR();
	g_ok.SetDisplayMode(OVRkill::StereoWithDistortion);
	g_ok.CreateShaders();
	g_ok.CreateRenderBuffer();

	running = true;
	initialised = true;
	
	return GameComponent::Initialise();
}

void Game::PrintText(string message, glm::vec2 position)
{
	messages.push_back(PrintMessage(message, position));
}

void Game::PrintText(string message)
{
	messages.push_back(PrintMessage(message, lastPrintPosition));
	lastPrintPosition.y += 20;
}

bool Game::Run() {
	
	if(Initialise() == false) {
        return false;
    }
	long last = SDL_GetTicks();
	while(running) {
		long now = SDL_GetTicks();
		float ellapsed = ((float) (now - last)) / 1000.0f;
		Update(ellapsed);
		PreDraw();
        Draw();
		PostDraw();
		last = now;
    }
 
	g_ok.DestroyOVR();
    Cleanup();
 
    return 0;
}

void Game::SetGround(shared_ptr<Ground> ground)
{
	children.remove(this->ground);
	this->ground = ground;
	AddChild(ground);
}

void Game::Update(float timeDelta) {
	// Check for messages
	SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        // Check for the quit message
        if (event.type == SDL_QUIT)
        {
        // Quit the program
        Cleanup();
		exit(0);
        }
    }

	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		Cleanup();
		exit(0);
	}
	PrintText(riftMessage);

	GameComponent::Update(timeDelta);
}

void Game::PreDraw()
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Game::PostDraw()
{	
	// Printing has to be done last, so we batch up the print messages
	vector<PrintMessage>::iterator it = messages.begin();
	while (it != messages.end())
	{
		//Print(it->message, it->position);
		it ++;
	}
	messages.clear();
	lastPrintPosition.y = 0;
	SDL_GL_SwapWindow(mainwindow);
}

void Game::Cleanup () {
	GameComponent::Cleanup();
	
	SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();	
}

Game * Game::Instance() {
	return instance;
}

const Uint8 * Game::GetKeyState()
{
	return keyState;
}

SDL_Window * Game::GetMainWindow()
{
	return mainwindow;
}

glm::mat4 OVRToGLMat4(OVR::Matrix4f m)
{
	glm::mat4 ret;
	m.Transpose();
	memcpy(& ret, & m, sizeof(OVR::Matrix4f));
	return ret;
}

/// World modelview matrix
OVR::Matrix4f  View;
const OVR::Vector3f UpVector     (0.0f, 1.0f, 0.0f);
const OVR::Vector3f ForwardVector(0.0f, 0.0f, -1.0f);
const OVR::Vector3f RightVector  (1.0f, 0.0f, 0.0f);

// We start out looking in the positive Z (180 degree rotation).
const float    YawInitial  = 3.141592f;
const float    Sensitivity = 1.0f;
const float    MoveSpeed   = 3.0f; // m/s

const float g_standingHeight = 1.78f; /// From Oculus SDK p.13: 1.78m ~= 5'10"
const float g_crouchingHeight = 0.6f;
OVR::Vector3f EyePos(0.0f, g_standingHeight, -5.0f);
float EyeYaw = YawInitial;
float EyePitch = 0;
float EyeRoll = 0;
float LastSensorYaw = 0;
OVR::Vector3f FollowCamPos(EyePos.x, EyePos.y + 3.0f, EyePos.z + 3.0f);
bool UseFollowCam = false;

void AccumulateInputs()
{
	// Handle Sensor motion.
	// We extract Yaw, Pitch, Roll instead of directly using the orientation
	// to allow "additional" yaw manipulation with mouse/controller.
	if (g_ok.SensorActive())
	{
		OVR::Quatf    hmdOrient = g_ok.GetOrientation();
		float    yaw = 0.0f;
		Game::Instance()->camera->orientation = RiftController::OVRToGLQuat(hmdOrient);

		hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &EyePitch, &EyeRoll);

		EyeYaw += (yaw - LastSensorYaw);
		LastSensorYaw = yaw;
	}
}

void AssembleViewMatrix()
{
	// Rotate and position View Camera, using YawPitchRoll in BodyFrame coordinates.
	// 
	OVR::Matrix4f rollPitchYaw = OVR::Matrix4f::RotationY(EyeYaw) *
		OVR::Matrix4f::RotationX(EyePitch) *
		OVR::Matrix4f::RotationZ(EyeRoll);
	OVR::Vector3f up      = rollPitchYaw.Transform(UpVector);
	OVR::Vector3f forward = rollPitchYaw.Transform(ForwardVector);

	// Minimal head modelling.
	float headBaseToEyeHeight     = 0.15f;  // Vertical height of eye from base of head
	float headBaseToEyeProtrusion = 0.09f;  // Distance forward of eye from base of head

	OVR::Vector3f eyeCenterInHeadFrame(0.0f, headBaseToEyeHeight, -headBaseToEyeProtrusion);
	OVR::Vector3f shiftedEyePos = EyePos + rollPitchYaw.Transform(eyeCenterInHeadFrame);
	shiftedEyePos.y -= eyeCenterInHeadFrame.y; // Bring the head back down to original height

	View = OVR::Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + forward, up); 

	// This is what transformation would be without head modeling.
	// View = Matrix4f::LookAtRH(EyePos, EyePos + forward, up);

	if (UseFollowCam)
	{
		OVR::Vector3f viewTarget(EyePos);
		OVR::Vector3f viewVector = viewTarget - FollowCamPos;
		View = OVR::Matrix4f::LookAtRH(FollowCamPos, viewVector, up); 
	}
}

void Game::Draw()
{	
	if (renderToRift)
	{
		AccumulateInputs();
		AssembleViewMatrix();
		glEnable(GL_DEPTH_TEST);
		g_ok.BindRenderBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const int fboWidth = g_ok.GetRenderBufferWidth();
		const int fboHeight = g_ok.GetRenderBufferHeight();
		const int halfWidth = fboWidth/2;
		const OVR::HMDInfo& hmd = g_ok.GetHMD();
		// Compute Aspect Ratio. Stereo mode cuts width in half.
		float aspectRatio = float(hmd.HResolution * 0.5f) / float(hmd.VResolution);

		// Compute Vertical FOV based on distance.
		float halfScreenDistance = (hmd.VScreenSize / 2);
		float yfov = 2.0f * atan(halfScreenDistance/hmd.EyeToScreenDistance);

		// Post-projection viewport coordinates range from (-1.0, 1.0), with the
		// center of the left viewport falling at (1/4) of horizontal screen size.
		// We need to shift this projection center to match with the lens center.
		// We compute this shift in physical units (meters) to correct
		// for different screen sizes and then rescale to viewport coordinates.
		float viewCenterValue = hmd.HScreenSize * 0.25f;
		float eyeProjectionShift = viewCenterValue - hmd.LensSeparationDistance * 0.5f;
		float projectionCenterOffset = 4.0f * eyeProjectionShift / hmd.HScreenSize;

		// Projection matrix for the "center eye", which the left/right matrices are based on.
		OVR::Matrix4f projCenter = OVR::Matrix4f::PerspectiveRH(yfov, aspectRatio, 0.3f, 1000.0f);
		OVR::Matrix4f projLeft   = OVR::Matrix4f::Translation(projectionCenterOffset, 0, 0) * projCenter;
		OVR::Matrix4f projRight  = OVR::Matrix4f::Translation(-projectionCenterOffset, 0, 0) * projCenter;

		// View transformation translation in world units.
		float halfIPD = hmd.InterpupillaryDistance * 0.5f;
		OVR::Matrix4f viewLeft = OVR::Matrix4f::Translation(halfIPD, 0, 0) * View;
		OVR::Matrix4f viewRight= OVR::Matrix4f::Translation(-halfIPD, 0, 0) * View;

		glViewport(0        ,0,(GLsizei)halfWidth, (GLsizei)fboHeight);
		glScissor (0        ,0,(GLsizei)halfWidth, (GLsizei)fboHeight);
		camera->view = OVRToGLMat4(viewLeft);
		camera->projection = OVRToGLMat4(projLeft);
		GameComponent::Draw();

		glViewport(halfWidth,0,(GLsizei)halfWidth, (GLsizei)fboHeight);
		glScissor (halfWidth,0,(GLsizei)halfWidth, (GLsizei)fboHeight);
		camera->view = OVRToGLMat4(viewRight);
		camera->projection = OVRToGLMat4(projRight);
		GameComponent::Draw();

		g_ok.UnBindRenderBuffer();
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		g_ok.PresentFbo();

	}
	else
	{
		glViewport(0, 0, width, height);
		GameComponent::Draw();
	}

	
}


int Game::GetWidth()
{
	return width;
}


int Game::GetHeight()
{
	return height;
}

void Game::Print(string message, glm::vec2 position)
{
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> uvs;

	
	SDL_Color foregroundColor = { 255, 255, 255 }; 
	SDL_Color backgroundColor = { 0, 0, 0 }; 
	SDL_Rect TextLocation;
	GLuint texture;
	GLint  colors;
	GLenum texture_format;
	SDL_Surface * surface = TTF_RenderText_Blended(font, message.c_str(), foregroundColor);
	colors = surface->format->BytesPerPixel;
	if (colors == 4) {
		if (surface->format->Rmask == 0x000000ff) {
			texture_format = GL_RGBA;
		}
		else {
			texture_format = GL_BGRA;
		}
	} else if (colors == 3) {
		if (surface->format->Rmask == 0x000000ff) {
				texture_format = GL_RGB;
		}
		else {
			texture_format = GL_BGR;
		}
	} else {
		printf("warning: the image is not truecolor..  this will probably break\n");
	}

	float x, y;
	float width = surface->w;
	float height = surface->h;

	x = position.x;
	y = (this->height - height) - position.y;
	
	
	vertices.push_back(glm::vec2(x + width,y));
	vertices.push_back(glm::vec2(x,y + height));
	vertices.push_back(glm::vec2(x,y));
	
	vertices.push_back(glm::vec2(x + width,y + height));
	vertices.push_back(glm::vec2(x,y + height));
	vertices.push_back(glm::vec2(x + width,y));
	
	
	uvs.push_back(glm::vec2(1, 1));
	uvs.push_back(glm::vec2(0, 0));
	uvs.push_back(glm::vec2(0, 1));

	uvs.push_back(glm::vec2(1, 0));	
	uvs.push_back(glm::vec2(0, 0));
	uvs.push_back(glm::vec2(1, 1));
	
	
	GLuint programID = Content::LoadShaderPair("TextShader");
	glUseProgram(programID);

	// Set our "myTextureSampler" sampler to user Texture Unit 0
	GLuint textureSampler  = glGetUniformLocation(programID, "myTextureSampler");

	//
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); 
	glTexImage2D(GL_TEXTURE_2D, 0, colors, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint sizeID = glGetUniformLocation(programID, "screen_size");
	glUniform2f(sizeID, this->width, this->height);
	
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureSampler, 0);
	
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);	
		
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 1st attribute buffer : normals
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_TRUE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteTextures(1, &texture);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	SDL_FreeSurface(surface);
}