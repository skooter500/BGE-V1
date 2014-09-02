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
#include<vector>
#include<string>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <memory>
#include "Exception.h"
#include "Transform.h"
#include <map>

using namespace std;

namespace BGE 
{
	float RandomFloat();

	class GameComponent:public std::enable_shared_from_this<GameComponent>
	{
	private:		
		void RotateVectors();
	protected:
		
	public:
		GameComponent(bool hasTransform = false);
		virtual ~GameComponent();
		virtual bool Initialise();
		virtual void Update(float timeDelta);
		virtual void PreDraw();
		virtual void Draw();
		virtual void PostDraw();
		virtual void Cleanup();

		void SetAlive(bool alive);

		shared_ptr<GameComponent> parent;
		std::string tag;
		bool transformOwner;

		std::list<std::shared_ptr<GameComponent>> children;
		std::multimap<std::string, std::shared_ptr<GameComponent>> childrenMap;

		float speed;
		bool alive;
		bool initialised;	
		bool isRelative; 

		std::shared_ptr<Transform> transform;
		std::shared_ptr<Transform> relativeTransform; 

		void Attach(std::shared_ptr<GameComponent> child);
		std::list<std::shared_ptr<GameComponent>> * GetChildren();

		int ClearAllChildren();
		int ClearChildrenWithTag(string tag);
		void TransformChildren(shared_ptr<Transform> transform);
		void TransformChildren(glm::mat4 mat);
		void InverseTransformChildren(shared_ptr<Transform> childTransform);
		shared_ptr<GameComponent> GameComponent::FindComponentByTag(string tag);
		std::vector<std::shared_ptr<GameComponent>> GameComponent::FindComponentsByTag(string tag);

		std::shared_ptr<GameComponent> This();
	};
}


