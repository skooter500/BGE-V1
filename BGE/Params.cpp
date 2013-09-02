#include "Params.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Content.h"
#include "Exception.h"
#include "Utils.h"

using namespace BGE;

map<string, string> Params::dictionary = map<string, string>();

Params::Params(void)
{
}

Params::~Params(void)
{
}

float Params::GetFloat(string key)
{
	map<string, string>::iterator it = dictionary.find(key);
	if (it == dictionary.end())
	{
		string message = "No value found for key: " + key + " so using default of 0";
		LogMessage(message);
		return 0.0f;
	}
	else
	{
		return atof(it->second.c_str());
	}
}
float Params::GetWeight(string key)
{
	map<string, string>::iterator it = dictionary.find(key);
	if (it == dictionary.end())
	{
		string message = "No value found for key: " + key + " so using default of 0";
		LogMessage(message);
		return 0.0f;
	}
	else
	{
		return atof(it->second.c_str()) * GetFloat("steering_weight_tweaker");
	}
}

string Params::Get(string key)
{
	return dictionary[key];
}



void Params::Load(string name)
{
	ifstream is;

	string filename = Content::prefix + name + ".properties";	
	LogMessage("Loading params file: " + name);

	is.open(filename, ios::in);

	if (is.is_open())
	{
		string s;
		while(!is.eof())
		{
			getline(is,s);
			string key, val;
			stringstream ss(s);
			getline(ss, key, '=');
			getline(ss, val);
			dictionary[trim(key)] = trim(val);
		}
	}
	else
	{
		throw BGE::Exception(("Could not load params file: " + filename).c_str());
	}
}
