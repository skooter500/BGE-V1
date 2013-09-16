#include "KinectFlyingController.h"
#include "Utils.h"
#include "VRGame.h"

using namespace BGE;

void CALLBACK BGE::StatusProc1( HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void * pUserData)
{      
	KinectFlyingController * kfc = (KinectFlyingController *) pUserData;
	if ( SUCCEEDED( hrStatus ) )      
	{   kfc->CreateFirstConnected();       
	kfc->connected = true;      
	}      
	else      
	{          
		kfc->tracked = false;
		kfc->connected = false;       
	}
}

float leftDiffZ = 0.0f;
float rightDiffZ = 0.0f;
float shoulderZ = 0.0f;


KinectFlyingController::KinectFlyingController(shared_ptr<Model> model)
{
	this->steerable = make_shared<Steerable3DController>(model);

	connected = false;
	tracked = false;
	m_pNuiSensor = NULL;
	scale = 20.0f;
	footHeight = 0.0f;
	myRoll = 0.0f;
	worldMode = world_modes::to_parent;

	//Attach(this->steerable);
}

KinectFlyingController::~KinectFlyingController(void)
{
}

void KinectFlyingController::Update(float timeDelta)
{
	this->timeDelta = timeDelta;

	Game::Instance()->PrintFloat("Left Z", leftDiffZ);
	Game::Instance()->PrintFloat("Right Z", rightDiffZ);
	Game::Instance()->PrintFloat("Shoulder Z", shoulderPos.z);

	if (connected)
	{
		SetStatusMessage("Kinect is connected");
		SetStatusMessage("Press C to toggle the head camera");
		if (tracked)
		{
			SetStatusMessage("Kinect is tracking");
		}
		else
		{
			SetStatusMessage("Kinect is not tracking");
		}
		// Wait for 0ms, just quickly test if it is time to process a skeleton
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextSkeletonEvent, 0) )
		{
			NUI_SKELETON_FRAME skeletonFrame = {0};

			// Get the skeleton frame that is ready
			if (SUCCEEDED(m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame)))
			{
				m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, &verySmoothParams);
				// Process the skeleton frame				
				SkeletonFrameReady(&skeletonFrame);
			}
		}
	}
	else
	{
		SetStatusMessage("Kinect is not connected");
	}
	steerable->Update(timeDelta);
	position = steerable->position;
	look = steerable->look;
	up = steerable->up;
	right = steerable->right;

	orientation= steerable->orientation;

	Game::Instance()->PrintVector("Steerable Position: ", position);
	GameComponent::Update(timeDelta);
}

void KinectFlyingController::SkeletonFrameReady( NUI_SKELETON_FRAME* pSkeletonFrame )
{
	tracked = false;
	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		const NUI_SKELETON_DATA & skeleton = pSkeletonFrame->SkeletonData[i];

		switch (skeleton.eTrackingState)
		{
		case NUI_SKELETON_TRACKED:
			UpdateSkeleton(skeleton);
			tracked = true;
			// Just draw the first skeleton I find
			return;
		case NUI_SKELETON_NOT_TRACKED:
		case NUI_SKELETON_POSITION_ONLY:
			break;
		}
	}

}

bool KinectFlyingController::Initialise()
{
	CreateFirstConnected();
	NuiSetDeviceStatusCallback(&StatusProc1, this);

	VRGame * game = (VRGame *) Game::Instance();

	this->steerable->worldMode = world_modes::from_self;
	this->steerable->position = position;
	
	steerable->position = position;
	steerable->look = look;
	steerable->up = up;
	steerable->right = right;
	return GameComponent::Initialise();
}

HRESULT KinectFlyingController::CreateFirstConnected()
{
	INuiSensor * pNuiSensor = NULL;

	int iSensorCount = 0;
	HRESULT hr = NuiGetSensorCount(&iSensorCount);
	if (FAILED(hr))
	{
		return hr;
	}

	// Look at each Kinect sensor
	for (int i = 0; i < iSensorCount; ++i)
	{
		// Create the sensor so we can check status, if we can't create it, move on to the next
		hr = NuiCreateSensorByIndex(i, &pNuiSensor);
		if (FAILED(hr))
		{
			continue;
		}

		// Get the status of the sensor, and if connected, then we can initialize it
		hr = pNuiSensor->NuiStatus();
		if (S_OK == hr)
		{
			m_pNuiSensor = pNuiSensor;
			break;
		}

		// This sensor wasn't OK, so release it since we're not using it
		pNuiSensor->Release();
		return hr;
	}

	if (NULL != m_pNuiSensor)
	{
		// Initialize the Kinect and specify that we'll be using skeleton
		hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON); 
		if (SUCCEEDED(hr))
		{
			// Create an event that will be signaled when skeleton data is available
			m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
			// Open a skeleton stream to receive skeleton data
			hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0); 
		}
	}

	if (NULL == m_pNuiSensor || FAILED(hr))
	{
		SetStatusMessage("No ready Kinect found!");
		return E_FAIL;
	}
	connected = true;
	SetStatusMessage("Kinect Connected");
	return hr;
}


void KinectFlyingController::SetStatusMessage( std::string message )
{
	Game::Instance()->PrintText(message);
}


void KinectFlyingController::UpdateSkeleton(const NUI_SKELETON_DATA & skeleton)
{
	const Uint8 * keyState = Game::Instance()->GetKeyState();
	static bool lastPressed = false;

	VRGame * game = (VRGame *) Game::Instance();

	if (!game->high)
	{
		return;
	}


	//if (footHeight == 0.0f)
	{
		footHeight = glm::min<float>(skeleton.SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT].y, skeleton.SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT].y);
	}	

	leftHandPos = NUIToGLVector(skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT], false);
	rightHandPos = NUIToGLVector(skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT], false);
	shoulderPos = NUIToGLVector(skeleton.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER], false);

	float forceScale = 1000.0f;




	//SDL_Joystick *joy;
	//if (SDL_NumJoysticks() > 0) {
	//	// Open joystick
	//	joy = SDL_JoystickOpen(0);
	//	if (joy) {
	//		stringstream ss;
	//		ss << SDL_JoystickNameForIndex(0) << " detected";
	//		Game::Instance()->PrintText(ss.str());

	//		float range = 1;

	//		int walkAxis = SDL_JoystickGetAxis(joy, 1);
	//		//CheckOverflow(walkAxis);
	//		if (glm::abs<int>(walkAxis) > 8000)
	//		{
	//			float walk = ((float) walkAxis / (float) numeric_limits<short int>::max()) * range;
	//			steerable->AddForce(look * walk * forceScale);
	//		}
	//	}
	//}

	// Close if opened
	/*if (SDL_JoystickGetAttached(joy)) {
		SDL_JoystickClose(joy);
	}*/

	float leftDiffY = leftHandPos.y - shoulderPos.y;
	float rightDiffY = rightHandPos.y - shoulderPos.y;

	leftDiffZ = leftHandPos.z;
	rightDiffZ = rightHandPos.z;

	if ((leftDiffY > 0.0f) && (rightDiffY > 0.0f))
	{
		float forceAmount = glm::max(leftDiffY, rightDiffY) * 20.0f;
		steerable->AddForce(glm::vec3(0, forceAmount * forceScale * timeDelta, 0));
		steerable->gravity = glm::vec3(0,0,0);

		float threashhold = 0.5f;
		leftDiffZ = shoulderPos.z - leftHandPos.z;
		rightDiffZ = shoulderPos.z - rightHandPos.z;
		if ((rightDiffZ > threashhold) )
		{
			float forceAmount = glm::max(leftDiffZ, rightDiffZ) * 100.0f;
			Game::Instance()->PrintFloat("Force AMount", forceAmount);
			steerable->AddForce(Game::Instance()->camera->combinedLook * forceAmount * forceScale * timeDelta);
			steerable->gravity = glm::vec3(0,0,0);		
		}
		return;		
	}
	if ((leftDiffY > 0.0f))
	{
		float forceAmount = - (leftHandPos.y - rightHandPos.y);
		steerable->AddTorque(up * forceAmount * forceScale * timeDelta * 0.50f);
		steerable->gravity = glm::vec3(0,0,0);
		return;
	}
	if ((rightDiffY > 0.0f))
	{
		float forceAmount = (rightHandPos.y - leftHandPos.y);
		myRoll = forceAmount;
		steerable->AddTorque(up * forceAmount * forceScale * timeDelta * 0.50f);
		
		return;
	}
	steerable->gravity = glm::vec3(0.0f, -9.0f, 0.0f);
}
