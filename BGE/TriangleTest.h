#pragma once
#include "Game.h"

namespace BGE
{
	class TriangleTest :
		public Game
	{
	public:
		TriangleTest(void);
		~TriangleTest(void);

		bool Initialise();
		void Draw();

	private:
		GLuint vertexbuffer;
		GLuint programID;
	};
}

