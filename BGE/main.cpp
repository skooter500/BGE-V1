
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
#include "VRGame2.h"
#include "Game.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <ctime>

#include <OVR.h>
#include <OVR_CAPI_GL.h>

#include "Content.h"
#include "FPSController.h"
#include "XBoxController.h"
#include "Steerable3DController.h"
#include "Utils.h"
#include "SDL_syswm.h"

using namespace BGE;

int main(int argc, char *argv[])
{
	// Check out the Kinect1/Kinect2 branches to get this demo:	
	// http://www.youtube.com/watch?v=EEbVHxOkTxw
	SceneGraphGame  game;
	game.Run();

	return 0;
}