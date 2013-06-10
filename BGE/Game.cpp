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

	SDL_WarpMouse(Game::Instance()->GetWidth() / 2,Game::Instance()->GetHeight() / 2);

    running = true;
	camera = new Camera();

	return true;
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
	camera->Update(timeDelta);	
}

void Game::Cleanup () {
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
	SDL_GL_SwapBuffers();
}

int Game::LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {
 
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
 
    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
 
    GLint Result = GL_FALSE;
    int InfoLogLength;
 
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
 
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
 
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
 
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
 
    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
 
    return ProgramID;
}

int Game::GetWidth()
{
	return width;
}


int Game::GetHeight()
{
	return height;
}