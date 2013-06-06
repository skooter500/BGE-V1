#include<sdl.h>
#include <windows.h>
#include <GL/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include<glm.hpp>
#include "Game.h"
#include "TriangleTest.h"

using namespace BGE;

int main(int argc, char *argv[])
{
	TriangleTest game;

	game.Run();

	return 0;
}