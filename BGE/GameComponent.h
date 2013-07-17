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
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>

namespace BGE 
{
	float RandomFloat();

	class GameComponent {
	private:		
		void RotateVectors();
	protected:
		bool initialised;
		
	public:

		GameComponent(void);
		~GameComponent(void);

		virtual bool Initialise();
		virtual void Update(float timeDelta);
		virtual void Draw();
		virtual void Cleanup();

		GameComponent * parent;
		glm::vec3 position;
		glm::vec3 look;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 scale;
		glm::vec3 velocity;
		glm::mat4 world;
		glm::quat orientation;

		glm::vec3 ambient;
		glm::vec3 specular;
		glm::vec3 diffuse; // Will use either this of the array of colours depending on the drawMode setting

		enum draw_modes {materials, textured, single_material};

		draw_modes drawMode;
		
		std::list<GameComponent *> children;
		static const glm::vec3 basisUp;
		static const glm::vec3 basisLook;
		static const glm::vec3 basisRight;
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
