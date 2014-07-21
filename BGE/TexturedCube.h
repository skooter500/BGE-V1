#pragma once
#include "Game.h"


namespace BGE
{
	class TexturedCube :
		public Game
	{
	public:
		TexturedCube(void);
		~TexturedCube(void);

		bool Initialise();
		void Draw();
		void Cleanup();

	private:
		GLuint texture;
		GLuint textureSampler;
		GLuint vertexbuffer;
		GLuint texelbuffer;
		GLuint programID;
	};
}
