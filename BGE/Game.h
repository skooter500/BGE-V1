#pragma once
//#include<sdl.h>
#ifdef _WIN32
 #include <windows.h>
 #include <windowsx.h>
 #include <io.h>
 #include <OVR.h>
 #include "RiftController.h"
#endif 
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include<glm.hpp>
#include <gtc/matrix_transform.hpp>
#include<vector>
#include<sstream>
#include <SDL_ttf.h>
#include <SDL.h>
#include "Camera.h"
#include "Ground.h"
#include "SoundSystem.h"
#include "LineDrawer.h"
#include "PhysicsFactory.h"
#include <iostream> 

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

	

	class Game:
		public GameComponent
	{
	private:	
		bool running;
		
		static shared_ptr<Game> instance;
		
		TTF_Font *font; // Declare a SDL_ttf font 

		std::vector<PrintMessage> messages;
		glm::vec2 lastPrintPosition;
		float fontSize;
		void Print(string message, glm::vec2);

		btBroadphaseInterface* broadphase;

		// Set up the collision configuration and dispatcher
		btDefaultCollisionConfiguration * collisionConfiguration;
		btCollisionDispatcher * dispatcher;

		// The actual physics solver
		btSequentialImpulseConstraintSolver * solver;

	public:

		Game(void);
		~Game(void);

		
		bool PreInitialise();
		bool Initialise();
		void Update(float timeDelta);
		void PreDraw();
		void Draw();
		void PostDraw();
		void Cleanup();
		
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
		void PrintAll();

		static shared_ptr<Game> Instance();
		shared_ptr<Camera> camera;
		shared_ptr<Ground> ground;

		SDL_Window * window; /* Our window handle */
		bool fullscreen;
		bool console;
		bool hud;
		SDL_GLContext context; /* Our opengl context handle */

		// Physics stuff
		std::shared_ptr<PhysicsFactory> physicsFactory;
		btDiscreteDynamicsWorld * dynamicsWorld;

		void DeletePhysicsConstraints();

#ifdef _WIN32 
		shared_ptr<RiftController> riftController;
#endif 
		shared_ptr<SoundSystem> soundSystem;

		string riftMessage;
		bool riftEnabled;
		int width, height;
		float fps;
		float frame;
		const Uint8 * keyState;
		float elapsed;

	};
}

