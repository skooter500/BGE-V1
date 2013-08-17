#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include "Model.h"
#include <fmod.hpp>

using namespace std;

namespace BGE
{
	class Content
	{
		private:
			static string prefix;
			static map<string, shared_ptr<Model>> models;
			static map<string, GLuint> textures;
			static map<string, GLuint> shaders;			
			static map<string, FMOD::Sound * > sounds;			
		public:
			static shared_ptr<Model> LoadModel(string name, glm::mat4 localTransform = glm::mat4(1));
			static FMOD::Sound * LoadSound(string name, FMOD::System * system);
			static GLuint LoadShaderPair(string name);
			static GLuint LoadTexture(std::string path);

	};
}