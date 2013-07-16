#pragma once
#include "gamecomponent.h"
#include <vector>

using namespace std;

namespace BGE
{
	class Ground :
		public GameComponent
	{
	public:
		Ground(void);
		~Ground(void);
		
		bool Initialise();
		void Draw();
		void Cleanup();

		std::vector<glm::vec3> vertices; 	
		std::vector<glm::vec2> texels;
		float width, height;

	private:
		GLuint vertexbuffer;
		GLuint texelbuffer;
		GLuint programID;
		GLuint textureID;
		GLuint textureSampler;
		GLuint MVPID;
		GLuint ModelViewID;

		GLuint mID, vID, pID;
		GLuint m_3x3_inv_transp;
	};
}
