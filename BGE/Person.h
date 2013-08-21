#pragma once
#include "GameComponent.h"
#include "PhysicsController.h"
#include "NuiApi.h"
#include <string>
#include <map>

// Some smoothing with little latency (defaults).
// Only filters out small jitters.
// Good for gesture recognition in games.
const NUI_TRANSFORM_SMOOTH_PARAMETERS defaultParams =
{0.5f, 0.5f, 0.5f, 0.05f, 0.04f};

// Smoothed with some latency.
// Filters out medium jitters.
// Good for a menu system that needs to be smooth but
// doesn't need the reduced latency as much as gesture recognition does.
const NUI_TRANSFORM_SMOOTH_PARAMETERS somewhatLatentParams =
{0.5f, 0.1f, 0.5f, 0.1f, 0.1f};

// Very smooth, but with a lot of latency.
// Filters out large jitters.
// Good for situations where smooth data is absolutely required
// and latency is not an issue.
const NUI_TRANSFORM_SMOOTH_PARAMETERS verySmoothParams =
{0.7f, 0.3f, 1.0f, 1.0f, 1.0f};


using namespace std;

namespace BGE
{
	void CALLBACK StatusProc( HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData);

	struct Hand
	{
		glm::vec3 pos;
		glm::vec3 look;
	};

	class Person :
		public GameComponent
	{
	private:
		bool m_bSeatedMode;
		INuiSensor* m_pNuiSensor;
		HANDLE m_hNextSkeletonEvent;
		void SetStatusMessage(std::string message);
		void UpdateSkeleton(const NUI_SKELETON_DATA & skeleton);
		void SkeletonFrameReady( NUI_SKELETON_FRAME* skeletonFrame );
		void UpdateBone( const NUI_SKELETON_DATA & skeleton, NUI_SKELETON_POSITION_INDEX jointFrom, NUI_SKELETON_POSITION_INDEX jointTo);
		void UpdateHead( const NUI_SKELETON_DATA & skeleton, NUI_SKELETON_POSITION_INDEX joint);
		
		void UpdateHand( const NUI_SKELETON_DATA & skeleton, NUI_SKELETON_POSITION_INDEX jointFrom, NUI_SKELETON_POSITION_INDEX jointTo, int handIndex);
		
		map<string, std::shared_ptr<PhysicsController>> boneComponents;
		bool connected;
		bool tracked;
		float footHeight;
		float scale;
	public:
		Person(void);
		~Person(void);

		void Update(float timeDelta);
		bool Initialise();
		HRESULT CreateFirstConnected();
		bool headCamera;
		friend void CALLBACK StatusProc( HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData);

		Hand hands[2];
	};
}

