#pragma once
#include "GameComponent.h"

namespace BGE {
	class Camera:public GameComponent {
	public:
		Camera(void);
		~Camera(void);
		void Update(float timeDelta);
		void Draw();
		bool Initialise();
		
		glm::vec3 startPos;

		glm::mat4 view;
		glm::mat4 projection;
	};
}

