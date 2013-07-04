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
		GLuint colourbuffer;
		GLuint programID;

		GLuint mID, vID, pID;
		GLuint m_3x3_inv_transp;

	public:
		Model();
		~Model();

		bool Initialise();
		void Update(float timeDelta);
		void Draw();

		std::vector<glm::vec3> vertices; 	
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> colours;

	};
}

