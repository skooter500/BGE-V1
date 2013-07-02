#pragma once
#include "game.h"

namespace BGE
{
	GLuint LoadTexture(std::string path);
	GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
};