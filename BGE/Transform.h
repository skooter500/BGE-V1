#pragma once
#include<glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <iostream> 

namespace BGE
{
	class Transform
	{
	public:
		Transform(void);
		~Transform(void);

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
		
		Transform * parent; 
		glm::vec3 backupPosition; 
		glm::quat backupOrientation; 
		glm::mat4 backupWorld; 
		
		static const glm::vec3 basisUp;
		static const glm::vec3 basisLook;
		static const glm::vec3 basisRight;

		bool moved;

		void Strafe(float units); 
		void Fly(float units);    
		void Walk(float units);   

		void Pitch(float angle, bool limitRotation = true); // rotate on right vector
		void Yaw(float angle);   // rotate on up vector
		void Roll(float angle);  // rotate on look vector

		void RecalculateVectors();
		void RotateVectors();
		void Calculate();

	};

}

