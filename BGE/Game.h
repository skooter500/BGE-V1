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
#include<sstream>
#include <SDL_ttf.h>
#include <OVR.h>
#include "Camera.h"
#include "Ground.h"
#include "SoundSystem.h"
#include "RiftController.h"
#include "LineDrawer.h"

using namespace OVR;

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
	

	class Game:
		public GameComponent
	{
	private:	
		SDL_GLContext maincontext; /* Our opengl context handle */
		bool running;
		
		static Game * instance;
		
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
		shared_ptr<Ground> GetGround();
		void SetGround(shared_ptr<Ground> ground);
		const Uint8 * GetKeyState();
		SDL_Window * GetMainWindow();
		int GetWidth();
		int GetHeight();		
		bool Run();
		
		void PrintText(string message, glm::vec2 position);
		void PrintText(string message);
		void PrintVector(string message, glm::vec3 v);
		void PrintFloat(string message, float f);

		shared_ptr<Camera> camera;
		shared_ptr<Ground> ground;

		SDL_Window * mainwindow; /* Our window handle */
		bool fullscreen;
		bool console;
		bool hud;

		shared_ptr<RiftController> riftController;
		shared_ptr<SoundSystem> soundSystem;

		string riftMessage;
		bool riftEnabled;
		int width, height;
		float fps;
		const Uint8 * keyState;
		float elapsed;

	};
}

