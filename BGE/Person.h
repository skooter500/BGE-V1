#pragma once
#include "GameComponent.h"
#include "NuiApi.h"
#include <string>
#include <map>

using namespace std;

namespace BGE
{
	void CALLBACK StatusProc( HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData);

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
		map<string, GameComponent *> boneComponents;
		bool connected;
		float footHeight;
	public:
		Person(void);
		~Person(void);

		void Update(float timeDelta);
		bool Initialise();
		HRESULT CreateFirstConnected();

		friend void CALLBACK StatusProc( HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData);

	};
}

