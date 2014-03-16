//#include<sdl.h>
#ifdef _WIN32
 #include <windows.h>
#endif 
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include<glm.hpp>
#include "Game.h"
#include "TriangleTest.h"
#include "TexturedCube.h"
#include "CubeTest.h"
#include "PhysicsGame1.h"
#include "ModelTest.h"
#include "SteeringGame.h"
#include "SceneGraphGame.h"
#include "Lab5.h"
#include "Lab4.h"
#include "Lab6.h"
#include "Lab7.h"
#include "Lab8.h"
#include "Buddha.h"

using namespace BGE;

int main(int argc, char *argv[])
{
	
	//Lab5 game;

	// Uncomment this line to run the Rift/Connect/Physics demo as per 
	// http://www.youtube.com/watch?v=EEbVHxOkTxw
	SceneGraphGame  game;
	game.Run();

	return 0;
}
