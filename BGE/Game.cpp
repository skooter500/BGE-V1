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

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {
	   -1.0f, -1.0f, 0.0f,
	   1.0f, -1.0f, 0.0f,
	   0.0f,  1.0f, 0.0f,
	};

Game::Game(void) {
	running = false;
	console = true;
	width = 800;
	height = 600;
	Surf_Display = NULL;
}

Game::~Game(void) {
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

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		char msg[2048];
		sprintf(msg, "%s", glewGetErrorString(err));
		std::cerr << "Error:" << glewGetErrorString(err);
		return false;
	}
	
	glViewport(0, 0, width, height);

    running = true;
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

	SDL_Event Event; 
	while(SDL_PollEvent(&Event)) {
            switch(Event.type) {
				case SDL_KEYDOWN: {
					Cleanup();
					exit(0);
				}
			}
        }
}

void Game::Cleanup () {
}


void Game::Draw()
{
	SDL_GL_SwapBuffers();
}

int Game::LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
 
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
