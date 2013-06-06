#pragma once
#include<glm.hpp>
#include<glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace BGE {
	class GameComponent {
	public:
		GameComponent(void);
		~GameComponent(void);

		virtual bool Initialise() { return true; };
		virtual void Update(float timeDelta) = 0;
		virtual void Draw() { };
		virtual void Cleanup() { };

		glm::vec3 position;
		glm::vec3 look;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 velocity;
		glm::mat4 world;
		bool moved;
		float speed;

		void Strafe(float units); 
		void Fly(float units);    
		void Walk(float units);   		

		void Pitch(float angle); // rotate on right vector
		void Yaw(float angle);   // rotate on up vector
		void Roll(float angle);  // rotate on look vector
	};
}
