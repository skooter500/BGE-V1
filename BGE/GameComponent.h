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
#include<list>
#include<string>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <memory>
#include "Exception.h"

namespace BGE 
{
	float RandomFloat();

	class GameComponent
	{
	private:		
		void RotateVectors();
	protected:
		
	public:
		GameComponent(void);

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
		bool initialised;	
		std::string tag;

		enum draw_modes {materials, textured, single_material};
		enum world_modes {from_self, from_self_with_parent, from_child, to_parent, from_parent};

		draw_modes drawMode;
		world_modes worldMode;
		
		std::list<std::shared_ptr<GameComponent>> children;
		static const glm::vec3 basisUp;
		static const glm::vec3 basisLook;
		static const glm::vec3 basisRight;
		bool moved;
		float speed;
		bool alive;

		void Strafe(float units); 
		void Fly(float units);    
		void Walk(float units);   		

		void Pitch(float angle); // rotate on right vector
		void Yaw(float angle);   // rotate on up vector
		void Roll(float angle);  // rotate on look vector
		void Attach(std::shared_ptr<GameComponent> child);
		std::list<std::shared_ptr<GameComponent>> * GetChildren();
		std::shared_ptr<GameComponent> GetController();
		void RecalculateVectors();
		void UpdateFromParent();
	};
}
