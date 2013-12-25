#pragma once
#include "GameComponent.h"

using namespace std;

namespace BGE {
	class Camera:public GameComponent {
	public:
		Camera(void);
		~Camera(void);
		void Update(float timeDelta);
		void Draw();
		bool Initialise();
		void RearView(shared_ptr<GameComponent> followObject, float distance, float height, float look_distance, float interp);
		void PerspectiveView(shared_ptr<GameComponent> followObject, glm::vec3 distanceFromObject, float x_angle, float z_angle, float look_distance, float interp);
		void UnFollow();
		
		glm::mat4 view;
		glm::mat4 projection;

		//Camera type enum
		enum camera_types {free,follow_behind, follow_perspective};
		camera_types cameraType;
		
		struct _perspective { //Perspective camera Struct
			glm::vec3 fromObj; //Distance from the object
			float x_angle; //Angle at which we look at
			float z_angle; //Angle at which we look at
			float look_distance; //Look distance in front of the object
			float interp; //Interpolation between 0 and 0.99 the bigger the value the smoother the transition
		};
		_perspective pCamVars;

		struct _rear { //Rear Camera Struct
			float distance; //distance from the object
			float height; //height from the object
			float look_distance; //loo
			float interp;
		};
		_rear rCamVars;

		//Follow camera common Variables
		shared_ptr<GameComponent> followObject;
		glm::vec3 camPosFromObj;
		glm::vec3 centerAt;

	private:
		glm::vec3 GetDistanceWithAngle();
	};
}
