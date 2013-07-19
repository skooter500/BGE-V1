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
#include "Camera.h"

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

	class Game:
		public GameComponent
	{
	private:
		SDL_Window * mainwindow; /* Our window handle */
		SDL_GLContext maincontext; /* Our opengl context handle */
		bool running;
		bool console;
		int width, height;
		static Game * instance;
		Camera * camera;
		const Uint8 * keyState;
		
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
		Camera * GetCamera();
		const Uint8 * GetKeyState();
		SDL_Window * GetMainWindow();
		int GetWidth();
		int GetHeight();		
		bool Run();

		std::vector<LightSource> lights;
	};
}

