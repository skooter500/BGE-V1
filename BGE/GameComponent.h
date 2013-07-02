#pragma once
#include<glm.hpp>
#include<glm.hpp>
#include<sdl.h>
#include <windows.h>
#include <windowsx.h>
#include <io.h>
#include <GL/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include<glm.hpp>
#include <gtc/matrix_transform.hpp>
#include<list>
#include <gtc/matrix_transform.hpp>

namespace BGE {
	class GameComponent {
	public:

		GameComponent(void);
		~GameComponent(void);

		virtual bool Initialise();
		virtual void Update(float timeDelta);
		virtual void Draw();
		virtual void Cleanup();

		glm::vec3 position;
		glm::vec3 look;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 velocity;
		glm::mat4 world;

		std::list<GameComponent *> children;
		static const glm::vec3 globalUp;
		static const glm::vec3 basis;
		bool moved;
		float speed;

		void Strafe(float units); 
		void Fly(float units);    
		void Walk(float units);   		

		void Pitch(float angle); // rotate on right vector
		void Yaw(float angle);   // rotate on up vector
		void Roll(float angle);  // rotate on look vector
		void AddChild(GameComponent * child);
		std::list<GameComponent *> * GetChildren();
	};
}
