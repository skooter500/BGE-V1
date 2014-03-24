#pragma once

#include<glm.hpp>
//#include<sdl.h>
#ifdef _WIN32
 #include <windows.h> 
 #include <windowsx.h>
 #include <io.h>
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include<glm.hpp>
#include<list>
#include<string>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <memory>
#include "Exception.h"
#include "Transform.h"

using namespace std;

namespace BGE 
{
	float RandomFloat();

	class GameComponent
	{
	private:		
		void RotateVectors();
	protected:
		
	public:
		GameComponent(bool hasTransform = false);

		virtual bool Initialise();
		virtual void Update(float timeDelta);
		virtual void Draw();
		virtual void Cleanup();

		GameComponent * parent;
		std::string tag;

		std::list<std::shared_ptr<GameComponent>> children;
		
		float speed;
		bool alive;
		bool initialised;	
		bool isRelative; 

		std::shared_ptr<Transform> transform;
		std::shared_ptr<Transform> relativeTransform; 

		void Attach(std::shared_ptr<GameComponent> child);
		void AttachWithRelativePositioning(std::shared_ptr<GameComponent> child); 
		std::list<std::shared_ptr<GameComponent>> * GetChildren();

		shared_ptr<GameComponent> GameComponent::FindComponent(string tag);

	};
}


