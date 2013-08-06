#pragma once
#include<sdl.h>
#include <windows.h>
#include <windowsx.h>
#include <io.h>
#include <GL/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include<glm.hpp>
#include <gtc/matrix_transform.hpp>
#include<vector>
#include <SDL_ttf.h>
#include "Camera.h"
#include "Ground.h"

namespace BGE
{
	struct LightSource
	{
		glm::vec4 position;
		glm::vec4 diffuse;
		glm::vec4 specular;
		float constantAttenuation, linearAttenuation, quadraticAttenuation;
		float spotCutoff, spotExponent;
		glm::vec3 spotDirection;
	};

	struct PrintMessage
	{
		PrintMessage(string m, glm::vec2 p):message(m), position(p) {}
		string message;
		glm::vec2 position;
	};

	glm::vec3 RotateVector(glm::vec3, glm::quat);
	void Log(string message);

	class Game:
		public GameComponent
	{
	private:	
		SDL_GLContext maincontext; /* Our opengl context handle */
		bool running;
		
		int width, height;
		static Game * instance;
		shared_ptr<Camera> camera;
		shared_ptr<Ground> ground;
		const Uint8 * keyState;
		TTF_Font *font; // Declare a SDL_ttf font 

		std::vector<PrintMessage> messages;
		glm::vec2 lastPrintPosition;
		float fontSize;
		void Print(string message, glm::vec2);
	public:
		Game(void);
		~Game(void);

		bool Initialise();
		void Update(float timeDelta);
		void PreDraw();
		void Draw();
		void PostDraw();
		void Cleanup();
		static Game * Instance();
		shared_ptr<Camera> GetCamera();
		void SetCamera(shared_ptr<Camera> ground);
		shared_ptr<Ground> GetGround();
		void SetGround(shared_ptr<Ground> ground);
		const Uint8 * GetKeyState();
		SDL_Window * GetMainWindow();
		int GetWidth();
		int GetHeight();		
		bool Run();

		void PrintText(string message, glm::vec2 position);
		void PrintText(string message);

		SDL_Window * mainwindow; /* Our window handle */
		bool fullscreen;
		bool console;
	};
}

