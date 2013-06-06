#pragma once
#include "GameComponent.h"

namespace BGE {
	class Camera:public GameComponent {
	private:
		glm::mat4 view;
		glm::mat4 projection;
	public:
		Camera(void);
		~Camera(void);
		void Update(float timeDelta);
		
		glm::mat4 GetView();
		glm::mat4 GetProjection();
	};
}

