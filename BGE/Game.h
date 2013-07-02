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

#include "Camera.h"


namespace BGE
{
	class Game:
		public GameComponent
	{
	private:
		SDL_Surface*    Surf_Display;
		bool running;
		bool console;
		int width, height;
		static Game * instance;
		Camera * camera;
		Uint8 * keyState;
		
	public:
		Game(void);
		~Game(void);

		bool Initialise();
		void Update(float timeDelta);
		void Draw();
		void Cleanup();
		static Game * Instance();
		Camera * GetCamera();
		Uint8 * GetKeyState();
		int GetWidth();
		int GetHeight();
		
		bool Run();
	};
}

