#pragma once
#include <glm.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include<glm.hpp>
#include <vector>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include "GameComponent.h"


#define MAX_LINES 500


namespace BGE
{
	class LineDrawer
	{
	private:
		LineDrawer(void);
		static LineDrawer * instance;

	public:
		~LineDrawer(void);

		static LineDrawer * Instance();
		bool Initialise();
		static void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 colour);
		static void DrawArrowLine(glm::vec3 start, glm::vec3 end, glm::vec3 colour, glm::quat arrowQuat = glm::quat());
		void Draw();
		void CleanUp();

		GLuint programID;
		GLuint colourBufferID;
		GLuint vertexBufferID;
		GLuint vID, pID;

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> colours;
	};
}
