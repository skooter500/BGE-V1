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
#include "XBoxController.h"
#include "Steerable3DController.h"
#include "Utils.h"

using namespace BGE;

BGE::Game * Game::instance = NULL;

glm::vec3 BGE::RotateVector(glm::vec3 v, glm::quat q)
{
	glm::quat qinv = glm::inverse(q);
	glm::quat w = glm::quat(0, v.x, v.y, v.z);
	w = q * w * qinv;

	return glm::vec3(w.x, w.y, w.z);
}



Game::Game(void) {
	running = false;
	console = true;
	fullscreen = true;

	hud = true;
	width = 800;
	height = 600;
	riftEnabled = false;
	// Rift
	width = 1280;
	height = 800;
	mainwindow = NULL;
	instance = this;
	srand(time(0));
	elapsed = 10000.0f;

	lastPrintPosition = glm::vec2(0,0);
	fontSize = 14;	

	fps = 0;

	worldMode = world_modes::from_self;

	camera = make_shared<Camera>();
	soundSystem = make_shared<SoundSystem>();
	soundSystem->Initialise();
	Attach(camera);

}

Game::~Game(void) {
}

shared_ptr<Ground> Game::GetGround()
{
	return ground;
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

	if (! GLEW_VERSION_2_0)
	{
		throw BGE::Exception("OpenGL Shaders not supported. Upgrade your graphics card drivers");
	}
	
	glViewport(0, 0, width, height);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	SDL_GL_SetSwapInterval(1);

	if (TTF_Init() < 0)
	{
		throw BGE::Exception("Could not init TTF");
	}
	font = TTF_OpenFont("Content/arial.ttf",fontSize); // Open a font & set the font size

	if (riftEnabled)
	{
		shared_ptr<RiftController> riftController = make_shared<RiftController>();
		riftController->position = glm::vec3(0, 10, 10);
		this->riftController = riftController;
		camera->Attach(riftController);
	}
	else
	{
		shared_ptr<GameComponent> controller = make_shared<FPSController>();
		controller->position = glm::vec3(0, 10, 10);
		camera->Attach(controller);
	}

	LineDrawer::Instance()->Initialise();
	
	running = true;
	initialised = true;
	
	return GameComponent::Initialise();
}

void Game::PrintText(string message, glm::vec2 position)
{
	messages.push_back(PrintMessage(message, position));
}

void Game::PrintVector(string message, glm::vec3 v)
{
	stringstream ss;
	ss << message << v.x << " " << v.y << " " << v.z;
	PrintText(ss.str());
}

void Game::PrintFloat(string message, float f)
{
	stringstream ss;
	ss << message << f;
	PrintText(ss.str());
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
    Cleanup();
 
    return 0;
}

void Game::SetGround(shared_ptr<Ground> ground)
{
	children.remove(this->ground);
	this->ground = ground;
	Attach(ground);
}

void Game::Update(float timeDelta) {
	// Check for messages
	fps = 1.0f / timeDelta;
	PrintText("FPS: " + to_string((long long) fps));
	soundSystem->Update();

	static bool lastPressed = false;
	if (keyState[SDL_SCANCODE_I])
	{
		if (! lastPressed)
		{
			hud = ! hud;
		}
		lastPressed = true;
	}
	else
	{
		lastPressed = false;
	}	

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
	//PrintText(riftMessage);

	GameComponent::Update(timeDelta);
}

void Game::PreDraw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

}

void Game::PostDraw()
{	
	// Printing has to be done last, so we batch up the print messages
	if (hud)
	{
		vector<PrintMessage>::iterator it = messages.begin();
		while (it != messages.end())
		{
			Print(it->message, it->position);
			it ++;
		}
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

void Game::Draw()
{	
	PrintText("Press I to toggle info");
	if (riftEnabled)
	{
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_CULL_FACE);
		
		riftController->BindRenderBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const int fboWidth = riftController->GetRenderBufferWidth();
		const int fboHeight = riftController->GetRenderBufferHeight();
		const int halfWidth = fboWidth/2;
		const OVR::HMDInfo& hmd = riftController->GetHMD();
		// Compute Aspect Ratio. Stereo mode cuts width in half.
		float aspectRatio = float(hmd.HResolution * 0.5f) / float(hmd.VResolution);

		// Compute Vertical FOV based on distance.
		float halfScreenDistance = (hmd.VScreenSize / 2);
		float yfov = 2.0f * glm::atan<float>(halfScreenDistance/hmd.EyeToScreenDistance);

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

		glm::mat4 cameraCentreView = camera->view;
		// View transformation translation in world units.
		float halfIPD = hmd.InterpupillaryDistance * 0.5f;
		OVR::Matrix4f viewLeft = OVR::Matrix4f::Translation(halfIPD, 0, 0) * GLToOVRMat4(camera->view);
		OVR::Matrix4f viewRight= OVR::Matrix4f::Translation(-halfIPD, 0, 0) * GLToOVRMat4(camera->view);

		glViewport(0        ,0,(GLsizei)halfWidth, (GLsizei)fboHeight);
		glScissor (0        ,0,(GLsizei)halfWidth, (GLsizei)fboHeight);
		camera->view = OVRToGLMat4(viewLeft);
		camera->projection = OVRToGLMat4(projLeft);
		// Draw all my children
		LineDrawer::Instance()->Draw();
		GameComponent::Draw();

		glViewport(halfWidth,0,(GLsizei)halfWidth, (GLsizei)fboHeight);
		glScissor (halfWidth,0,(GLsizei)halfWidth, (GLsizei)fboHeight);
		camera->view = OVRToGLMat4(viewRight);
		camera->projection = OVRToGLMat4(projRight);
		// Draw all my children
		LineDrawer::Instance()->Draw();
		GameComponent::Draw();

		riftController->UnBindRenderBuffer();
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		riftController->PresentFbo();
		camera->view = cameraCentreView;
	}
	else
	{
		glViewport(0, 0, width, height);	
		LineDrawer::Instance()->Draw();
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