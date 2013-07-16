#pragma once
#include "GameComponent.h"
#include "Game.h"

#include <vector>

using namespace std;

namespace BGE
{

	class Model :
		public GameComponent
	{
		
	private:
		GLuint vertexbuffer;
		GLuint normalbuffer;
		GLuint uvbuffer;
		GLuint colourbuffer;
		GLuint ambientID;
		GLuint specularID;
		GLuint diffuseID;
		GLuint programID;
		GLuint diffusePerVertexID;
		GLuint mID, vID, pID, nID;

	public:
		Model();
		~Model();

		bool Initialise();
		void Draw();

		std::vector<glm::vec3> vertices; 	
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> uvs;
		std::vector<glm::vec3> colours;

		glm::vec3 ambient;
		glm::vec3 specular;
		glm::vec3 diffuse; // Will use either this of the array of colours depending on the drawMode setting

		enum draw_modes {materials, textured, single_material};

		draw_modes drawMode;

	};
}

