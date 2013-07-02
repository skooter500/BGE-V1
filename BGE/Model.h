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

		GLuint m_WVPLocation;
		GLuint m_WorldMatrixLocation;

		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint Direction;
			GLuint DiffuseIntensity;
		} m_dirLightLocation;

		DirectionalLight m_directionalLight;
		
	public:
		Model();
		~Model();
		void SetDirectionalLight(const DirectionalLight& Light);

		bool Initialise();
		void Update(float timeDelta);
		void Draw();

		std::vector<glm::vec3> vertices; 	
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> colours;

	};
}

