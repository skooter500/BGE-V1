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
#include "Steerable3DController.h"

using namespace BGE;

BGE::Game * Game::instance = NULL;

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
	console = true;
	fullscreen = false;
	width = 1024;
	height = 768;
	mainwindow = NULL;
	instance = this;
	srand(time(0));

	lastPrintPosition = glm::vec2(0,0);
	fontSize = 14;	

	worldMode = world_modes::from_self;

	camera = make_shared<Camera>();
	shared_ptr<GameComponent> controller = make_shared<FPSController>();
	controller->position = glm::vec3(0, 10, 10);
	camera->AddChild(controller);
	AddChild(camera);

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
	}// Initilize SDL_ttf
	font = TTF_OpenFont("Content/arial.ttf",fontSize); // Open a font & set the font size

	//shared_ptr<GameComponent> controller = make_shared<Steerable3DController>(Content::LoadModel("cube"));
	
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
		Print(it->message, it->position);
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

void Game::Draw()
{	
	GameComponent::Draw();	
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