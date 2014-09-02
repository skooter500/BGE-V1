#pragma once
#include <map>
#include <string>
#include<glm.hpp>

using namespace std;

namespace BGE
{
	class Params
	{
		private:
			Params(void);
			~Params(void);
		public:
			static map<string, string> dictionary;
			static float GetFloat(string key);
			static void SetFloat(string key, float value);
			static float GetWeight(string key);
			static bool GetBool(string key);
			static void SetBool(string key, bool value);
			static string Get(string key);
			static void Load(string filename);
			static bool ExistsKey(string key);
			static glm::vec3 GetVector3(string key);
	};
}
