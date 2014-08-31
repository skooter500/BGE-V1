#pragma once
#include "GameComponent.h"
#include "Game.h"

#include <vector>

using namespace std;

namespace BGE
{
	struct BoundingBox
	{
		glm::vec3 min;
		glm::vec3 max;
	};

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
		GLuint uvID;
		GLuint programID;
		GLuint diffusePerVertexID;
		GLuint mID, vID, pID, nID;
		GLuint mvpID;
		GLuint texelbuffer;
		GLuint textureSampler;
		GLuint textureID;

		void CalculateBounds();
		void UpdateFromParent();
	public:
		Model(); 
		~Model();

		bool Initialise();
		void Draw();
		void Update(float timeDelta);
		
		GLuint * dumpPrivateContents(void); 

		std::vector<glm::vec3> vertices; 	
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> colours;
		
		BoundingBox boundingBox;
		glm::mat4 localTransform;
		string textureName;

		enum draw_modes {materials, textured, single_material};
		draw_modes drawMode;
	};
}

