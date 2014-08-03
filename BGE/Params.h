#pragma once
#include <map>
#include <string>

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
			static float GetWeight(string key);
			static string Get(string key);
			static void Load(string filename);
	};
}
