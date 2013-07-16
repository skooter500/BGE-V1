#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include "Model.h"

using namespace std;

namespace BGE
{
	class Content
	{
		private:
			static string prefix;
		public:
			static Model * LoadModel(string name);
			static BGE::Model * BGE::Content::SimpleLoadModel(string name);
			static GLuint LoadShaderPair(string name);
			static GLuint LoadTexture(std::string path);

	};
}