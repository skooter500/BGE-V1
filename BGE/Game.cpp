#include "Game.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>

using namespace BGE;

BGE::Game * Game::instance = NULL;

Game::Game(void) {
	running = false;
	console = true;
	width = 800;
	height = 600;
	Surf_Display = NULL;
	instance = this;
}

Game::~Game(void) {
	delete camera;
}

Camera * Game::GetCamera()
{
	return camera;
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
	
	if((Surf_Display = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)) == NULL) {
		return false;
	}
	keyState = SDL_GetKeyState(NULL);
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
    running = true;
	camera = new Camera();
	AddChild(camera);

	return GameComponent::Initialise();
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
        Draw();
		last = now;
    }
 
    Cleanup();
 
    return 0;
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

	if (keyState[SDLK_ESCAPE])
	{
		Cleanup();
		exit(0);
	}
	GameComponent::Update(timeDelta);
}

void Game::Cleanup () {
	GameComponent::Cleanup();
}

Game * Game::Instance() {
	return instance;
}

Uint8 * Game::GetKeyState()
{
	return keyState;
}

void Game::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GameComponent::Draw();
	SDL_GL_SwapBuffers();
}


int Game::GetWidth()
{
	return width;
}


int Game::GetHeight()
{
	return height;
}