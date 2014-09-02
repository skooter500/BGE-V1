#pragma once
#include<glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <memory>
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
		glm::mat4 world;
		glm::mat4 worldNoScale;
		glm::quat orientation;
		glm::vec3 ambient;
		glm::vec3 specular;
		glm::vec3 diffuse; // Will use either this of the array of colours depending on the drawMode setting
		glm::vec3 velocity;
		static const glm::vec3 basisUp;
		static const glm::vec3 basisLook;
		static const glm::vec3 basisRight;

		std::shared_ptr<Transform> parent;

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

		glm::quat TransformOrientation(glm::quat in);
		glm::quat InverseTransformOrientation(glm::quat in);
		glm::vec3 TransformPosition(glm::vec3 in, bool scale = false);
		glm::vec3 TransformNormal(glm::vec3 in, bool scale = false);
		glm::vec3 InverseTransformPosition(glm::vec3 in, bool scale = false);
		glm::vec3 InverseTransformNormal(glm::vec3 in, bool scale = false);
	};

}

