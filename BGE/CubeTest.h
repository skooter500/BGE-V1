#pragma once
#include "game.h"

namespace BGE
{
	class CubeTest :
		public Game
	{
	public:
		CubeTest(void);
		~CubeTest(void);

		bool Initialise();
		void Draw();
		void Cleanup();

	private:
		GLuint vertexbuffer;
		GLuint colorbuffer;
		GLuint programID;
		GLuint VertexArrayID;
	};
}
