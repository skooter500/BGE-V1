#pragma once
#include "GameComponent.h"

#include <vector>

using namespace std;

namespace BGE
{
	class Model :
		public GameComponent
	{
		
	private:
		GLuint MatrixID;
		GLuint ViewMatrixID;
		GLuint ModelMatrixID;
		GLuint LightID;
		GLuint vertexbuffer;
		GLuint normalbuffer;
		GLuint colourbuffer;
		GLuint programID;
		
	public:
		Model();
		~Model();
		
		bool Initialise();
		void Draw();

		std::vector<glm::vec3> vertices; 	
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> colours;

	};
}

