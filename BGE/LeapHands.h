#pragma once
#include "GameComponent.h"
#include <Leap.h>
#include "SkeletonMapper.h"

using namespace Leap;

namespace BGE
{

	class LeapHands:public GameComponent, public Leap::Listener
	{
	private:
		CRITICAL_SECTION criticalSection;
	public:
		LeapHands();
		~LeapHands();

		virtual bool Initialise();
		virtual void Update(float timeDelta);
		virtual void Draw();
		virtual void Cleanup();

		Leap::Controller controller;

		shared_ptr<SkeletonMapper> mapper;

		enum spawn_type {none, vehicle, model};
		int trackedHands;
		spawn_type spawn = none;
		
		shared_ptr<Transform> handTransform;

		bool dataReady;
		void UpdateBone(string tag, glm::vec3 start, glm::vec3);

		virtual void onInit(const Controller&);
		virtual void onConnect(const Controller&);
		virtual void onDisconnect(const Controller&);
		virtual void onExit(const Controller&);
		virtual void onFrame(const Controller&);
		virtual void onFocusGained(const Controller&);
		virtual void onFocusLost(const Controller&);
		virtual void onDeviceChange(const Controller&);
		virtual void onServiceConnect(const Controller&);
		virtual void onServiceDisconnect(const Controller&);
		void TransformHand(float timeDelta);
		glm::vec3 TransformPointRelativeToCamera(glm::vec3 pos);
	};
}
