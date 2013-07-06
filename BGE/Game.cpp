#include "Game.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include "Utils.h"

using namespace BGE;

BGE::Game * Game::instance = NULL;

Game::Game(void) {
	running = false;
	console = true;
	width = 800;
	height = 600;
	mainwindow = NULL;
	instance = this;
	camera = NULL;
}

Game::~Game(void) {
	if (camera != NULL)
	{
		delete camera;
	}
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

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
 
    /* Create our window centered at 512x512 resolution */
    mainwindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	SDL_GL_SetSwapInterval(1);
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
		PreDraw();
        Draw();
		PostDraw();
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

	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		Cleanup();
		exit(0);
	}
	GameComponent::Update(timeDelta);
}

void Game::PreDraw()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Game::PostDraw()
{
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