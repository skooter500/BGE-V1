#include "Person2.h"
#include <sstream>
#include "Box.h"
#include "VRGame2.h"
#include "Cylinder.h"
#include "Utils.h"
#include "Content.h"
#include "PhysicsController.h"
#include "KinematicMotionState.h"
#include <btBulletDynamicsCommon.h>

using namespace BGE;
using namespace std;

LPCWSTR GrammarFileName = L"Content/bge.grxml";

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

Person2::Person2(void):GameComponent(true)
{
	connected = false;
	tracked = false;
	headCamera = false;
	scale = 20.0f;
	footHeight = 0.0f;
	tag = "Person2";
}


Person2::~Person2(void)
{
	// close the Kinect Sensor
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);

	//16 bit Audio Stream
	if (NULL != m_p16BitAudioStream)
	{
		delete m_p16BitAudioStream;
		m_p16BitAudioStream = NULL;
	}
	SafeRelease(m_pAudioStream);
	SafeRelease(m_pAudioBeam);
	SafeRelease(m_pKinectSensor);
}

bool Person2::Initialise()
{
	HRESULT hr;

	IAudioSource* pAudioSource = NULL;
	IAudioBeamList* pAudioBeamList = NULL;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_AudioSource(&pAudioSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pAudioSource->get_AudioBeams(&pAudioBeamList);
		}

		if (SUCCEEDED(hr))
		{
			hr = pAudioBeamList->OpenAudioBeam(0, &m_pAudioBeam);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pAudioBeam->OpenInputStream(&m_pAudioStream);
			m_p16BitAudioStream = new KinectAudioStream(m_pAudioStream);
		}

		connected = true;

		SafeRelease(pBodyFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		SetStatusMessage("No ready Kinect found!");
		connected = false;
		return false;
	}

	if (FAILED(hr))
	{
		SetStatusMessage("Failed opening an audio stream!");
		return hr;
	}

	hr = CoCreateInstance(CLSID_SpStream, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpStream), (void**)&m_pSpeechStream);

	// Audio Format for Speech Processing
	WORD AudioFormat = WAVE_FORMAT_PCM;
	WORD AudioChannels = 1;
	DWORD AudioSamplesPerSecond = 16000;
	DWORD AudioAverageBytesPerSecond = 32000;
	WORD AudioBlockAlign = 2;
	WORD AudioBitsPerSample = 16;

	WAVEFORMATEX wfxOut = { AudioFormat, AudioChannels, AudioSamplesPerSecond, AudioAverageBytesPerSecond, AudioBlockAlign, AudioBitsPerSample, 0 };

	if (SUCCEEDED(hr))
	{

		m_p16BitAudioStream->SetSpeechState(true);
		hr = m_pSpeechStream->SetBaseStream(m_p16BitAudioStream, SPDFID_WaveFormatEx, &wfxOut);
	}

	if (SUCCEEDED(hr))
	{
		hr = CreateSpeechRecognizer();
	}

	if (FAILED(hr))
	{
		SetStatusMessage("Could not create speech recognizer. Please ensure that Microsoft Speech SDK and other sample requirements are installed.");
		return hr;
	}

	hr = LoadSpeechGrammar();

	if (FAILED(hr))
	{
		SetStatusMessage("Could not load speech grammar. Please ensure that grammar configuration file was properly deployed.");
		return hr;
	}

	hr = StartSpeechRecognition();

	if (FAILED(hr))
	{
		SetStatusMessage("Could not start recognizing speech.");
		return hr;
	}

	m_bSpeechActive = true;

	SafeRelease(pAudioBeamList);
	SafeRelease(pAudioSource);

	return GameComponent::Initialise();
}

void Person2::UpdateBone(const Joint* pJoints, JointType joint0, JointType joint1)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;
	TrackingState joint1State = pJoints[joint1].TrackingState;

	// If we can't find either of these joints, exit
	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
	{
		return;
	}

	// Don't draw if both points are inferred
	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
	{
		return;
	}

	glm::vec3 start = Scale(KinectToGLVector(pJoints[joint0].Position));
	glm::vec3 end = Scale(KinectToGLVector(pJoints[joint1].Position));
	
	
	glm::vec3 boneVector = end - start;
	float boneLength = glm::length(boneVector);
	glm::vec3 centrePos = start + ((boneVector) / 2.0f);
	
	boneVector = glm::normalize(boneVector);
	//glm::quat q = RotationBetweenVectors(Transform::basisUp, boneVector);
	glm::vec3 axis = glm::cross(Transform::basisUp, boneVector);
	axis = glm::normalize(axis);
	float theta = (float) glm::acos<float>(glm::dot<float>(Transform::basisUp, boneVector));
	glm::quat q = glm::angleAxis(glm::degrees(theta), axis);

	stringstream ss;
	ss << joint0 << "," << joint1;
	string boneKey = ss.str();

	map<string, shared_ptr<PhysicsController>>::iterator it = boneComponents.find(boneKey);
	shared_ptr<PhysicsController> cylController;
	VRGame2 * game = (VRGame2 *)Game::Instance();
	if (it == boneComponents.end())
	{

		cylController = game->physicsFactory->CreateCylinder(0.5f, boneLength, centrePos, transform->orientation);
		cylController->rigidBody->setCollisionFlags(cylController->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		cylController->rigidBody->setActivationState(DISABLE_DEACTIVATION);
		cylController->rigidBody->setMotionState(new KinematicMotionState(cylController->parent));
		cylController->tag = "Bone";
		boneComponents[boneKey] = cylController;
	}
	else
	{
		cylController = it->second;
	}
	cylController->transform->position = this->transform->position + centrePos;
	cylController->parent->transform->orientation = q;
}

void Person2::UpdateHead(const Joint* pJoints, JointType joint0)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;

	// If we can't find either of these joints, exit
	if (joint0State == TrackingState_NotTracked)
	{
		return;
	}

	glm::vec3 start = Scale(KinectToGLVector(pJoints[joint0].Position));

	stringstream ss;
	ss << joint0;
	string boneKey = ss.str();

	VRGame2 * game = (VRGame2 *)Game::Instance();

	map<string, shared_ptr<PhysicsController>>::iterator it = boneComponents.find(boneKey);
	shared_ptr<PhysicsController> box;
	glm::quat q = glm::angleAxis(glm::degrees(glm::half_pi<float>()), glm::vec3(1, 0, 0));
	if (it == boneComponents.end())
	{
		
		box = game->physicsFactory->CreateCylinder(2.0f, 0.5f, start, q);
		box->rigidBody->setCollisionFlags(box->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		box->rigidBody->setActivationState(DISABLE_DEACTIVATION);
		box->rigidBody->setMotionState(new KinematicMotionState(box->parent));
		box->tag = "Head";

		boneComponents[boneKey] = box;
	}
	else
	{
		box = it->second;
	}

	if (headCamera)
	{
		game->camera->transform->position = start + glm::vec3(0, scale * 3.0f, 0);
		box->transform->position = glm::vec3(100, -100, 100);
		box->transform->orientation = q;
	}
	else
	{
		box->transform->position = this->transform->position + start;
		box->transform->orientation = q;
	}
}

void Person2::Update(float timeDelta)
{
	IBodyFrame* pBodyFrame = NULL;

	if (!m_pBodyFrameReader)
	{
		SetStatusMessage("Kinect 2 body frame reader is null");
		return;
	}

	HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

	if (SUCCEEDED(hr))
	{
		tracked = true;
		INT64 nTime = 0;
		hr = pBodyFrame->get_RelativeTime(&nTime);
		IBody* ppBodies[BODY_COUNT] = { 0 };
		
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}


		if (SUCCEEDED(hr))
		{
			ProcessBody(nTime, BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}
	SafeRelease(pBodyFrame);

	if (connected)
	{
		SetStatusMessage("Kinect is connected");
		SetStatusMessage("Press C to toggle the head camera");
		if (tracked)
		{
			SetStatusMessage("Kinect 2 is tracking");
		}
		else
		{
			SetStatusMessage("Kinect 2 is not tracking");
		}		
	}
	else
	{
		SetStatusMessage("Kinect 2 is not connected");
	}

	const Uint8 * keyState = Game::Instance()->GetKeyState();
	static bool lastPressed = false;
	if (keyState[SDL_SCANCODE_C])
	{
		if (!lastPressed)
		{
			headCamera = !headCamera;
		}
		lastPressed = true;
	}
	else
	{
		lastPressed = false;
	}

	if (m_bSpeechActive)
	{
		ProcessSpeech();
	}

	GameComponent::Update(timeDelta);
}

glm::vec3 Person2::Scale(glm::vec3 point)
{
	point.y -= footHeight;
	point *= scale;
	return point;
}

void Person2::UpdateHands(IBody * pBody, Joint * joints)
{
	HandState leftHandState = HandState_Unknown;
	HandState rightHandState = HandState_Unknown;

	pBody->get_HandLeftState(&leftHandState);
	pBody->get_HandRightState(&rightHandState);

	hands[0].pos = Scale(KinectToGLVector(joints[JointType_HandLeft].Position));
	hands[0].state = leftHandState;
	hands[0].look = glm::normalize(KinectToGLVector(joints[JointType_HandLeft].Position) - KinectToGLVector(joints[JointType_WristLeft].Position));
	Game::Instance()->PrintVector("Left hand look: ", hands[0].look);

	hands[1].pos = Scale(KinectToGLVector(joints[JointType_HandRight].Position));
	hands[1].state = rightHandState;
	hands[1].look = glm::normalize(KinectToGLVector(joints[JointType_HandRight].Position) - KinectToGLVector(joints[JointType_WristRight].Position));
	Game::Instance()->PrintVector("Right hand look: ", hands[0].look);
}

void Person2::ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies)
{	
	if (m_pCoordinateMapper && (nBodyCount > 0))
	{
		for (int i = 0; i < nBodyCount; ++i)
		{
			IBody * pBody = ppBodies[i];

			if (pBody)
			{			
				BOOLEAN bTracked = false;
				HRESULT hr = pBody->get_IsTracked(&bTracked);

				if (SUCCEEDED(hr) && bTracked)
				{
					Joint joints[JointType_Count];

					hr = pBody->GetJoints(_countof(joints), joints);
					UpdateHands(pBody, joints);
					if (SUCCEEDED(hr))
					{
						if (footHeight == 0.0f)
						{
							footHeight = glm::min<float>(joints[JointType_FootLeft].Position.Y, joints[JointType_FootLeft].Position.Y);
						}

						UpdateBone(joints, JointType_Head, JointType_Neck);
						UpdateBone(joints, JointType_Neck, JointType_SpineShoulder);
						UpdateBone(joints, JointType_SpineShoulder, JointType_SpineMid);
						UpdateBone(joints, JointType_SpineMid, JointType_SpineBase);
						UpdateBone(joints, JointType_SpineShoulder, JointType_ShoulderRight);
						UpdateBone(joints, JointType_SpineShoulder, JointType_ShoulderLeft);
						UpdateBone(joints, JointType_SpineBase, JointType_HipRight);
						UpdateBone(joints, JointType_SpineBase, JointType_HipLeft);

						// Right Arm    
						UpdateBone(joints, JointType_ShoulderRight, JointType_ElbowRight);
						UpdateBone(joints, JointType_ElbowRight, JointType_WristRight);
						UpdateBone(joints, JointType_WristRight, JointType_HandRight);
						UpdateBone(joints, JointType_HandRight, JointType_HandTipRight);
						//UpdateBone(joints, JointType_WristRight, JointType_ThumbRight);

						// Left Arm
						UpdateBone(joints, JointType_ShoulderLeft, JointType_ElbowLeft);
						UpdateBone(joints, JointType_ElbowLeft, JointType_WristLeft);
						UpdateBone(joints, JointType_WristLeft, JointType_HandLeft);
						UpdateBone(joints, JointType_HandLeft, JointType_HandTipLeft);
						//UpdateBone(joints, JointType_WristLeft, JointType_ThumbLeft);

						// Right Leg
						UpdateBone(joints, JointType_HipRight, JointType_KneeRight);
						UpdateBone(joints, JointType_KneeRight, JointType_AnkleRight);
						UpdateBone(joints, JointType_AnkleRight, JointType_FootRight);

						// Left Leg
						UpdateBone(joints, JointType_HipLeft, JointType_KneeLeft);
						UpdateBone(joints, JointType_KneeLeft, JointType_AnkleLeft);
						UpdateBone(joints, JointType_AnkleLeft, JointType_FootLeft);

						UpdateHead(joints, JointType_Head);
					}
				}
			}
		}
	}
}

void Person2::SetStatusMessage( std::string message )
{
	Game::Instance()->PrintText(message);
}

// Kinect Speech stuff
// Straight from the SDK examples
HRESULT Person2::CreateSpeechRecognizer()
{
	ISpObjectToken *pEngineToken = NULL;

	HRESULT hr = CoCreateInstance(CLSID_SpInprocRecognizer, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpRecognizer), (void**)&m_pSpeechRecognizer);

	if (SUCCEEDED(hr))
	{
		m_pSpeechRecognizer->SetInput(m_pSpeechStream, TRUE);

		// If this fails here, you have not installed the acoustic models for Kinect
		hr = SpFindBestToken(SPCAT_RECOGNIZERS, L"Language=409;Kinect=True", NULL, &pEngineToken);

		if (SUCCEEDED(hr))
		{
			m_pSpeechRecognizer->SetRecognizer(pEngineToken);
			hr = m_pSpeechRecognizer->CreateRecoContext(&m_pSpeechContext);

			// For long recognition sessions (a few hours or more), it may be beneficial to turn off adaptation of the acoustic model. 
			// This will prevent recognition accuracy from degrading over time.
			if (SUCCEEDED(hr))
			{
				hr = m_pSpeechRecognizer->SetPropertyNum(L"AdaptationOff", 0);
			}
		}
	}
	SafeRelease(pEngineToken);
	return hr;
}

HRESULT Person2::LoadSpeechGrammar()
{
	HRESULT hr = m_pSpeechContext->CreateGrammar(1, &m_pSpeechGrammar);

	if (SUCCEEDED(hr))
	{
		// Populate recognition grammar from file
		hr = m_pSpeechGrammar->LoadCmdFromFile(GrammarFileName, SPLO_STATIC);
	}

	return hr;
}

HRESULT Person2::StartSpeechRecognition()
{
	HRESULT hr = S_OK;

	// Specify that all top level rules in grammar are now active
	hr = m_pSpeechGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);
	if (FAILED(hr))
	{
		return hr;
	}

	// Specify that engine should always be reading audio
	hr = m_pSpeechRecognizer->SetRecoState(SPRST_ACTIVE_ALWAYS);
	if (FAILED(hr))
	{
		return hr;
	}

	// Specify that we're only interested in receiving recognition events
	hr = m_pSpeechContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
	if (FAILED(hr))
	{
		return hr;
	}

	// Ensure that engine is recognizing speech and not in paused state
	hr = m_pSpeechContext->Resume(0);
	if (FAILED(hr))
	{
		return hr;
	}

	m_hSpeechEvent = m_pSpeechContext->GetNotifyEventHandle();
	return hr;
}

HRESULT Person2::ProcessSpeech()
{
	const float ConfidenceThreshold = 0.8f;

	SPEVENT curEvent = { SPEI_UNDEFINED, SPET_LPARAM_IS_UNDEFINED, 0, 0, 0, 0 };
	ULONG fetched = 0;
	HRESULT hr = S_OK;

	m_pSpeechContext->GetEvents(1, &curEvent, &fetched);
	while (fetched > 0)
	{
		switch (curEvent.eEventId)
		{
		case SPEI_RECOGNITION:
			if (SPET_LPARAM_IS_OBJECT == curEvent.elParamType)
			{
				// this is an ISpRecoResult
				ISpRecoResult* result = reinterpret_cast<ISpRecoResult*>(curEvent.lParam);
				SPPHRASE* pPhrase = NULL;

				hr = result->GetPhrase(&pPhrase);				
				if (SUCCEEDED(hr))
				{					
					if ((pPhrase->pProperties != NULL) && (pPhrase->pProperties->Confidence > ConfidenceThreshold))
					{
						PerformAction(pPhrase->pProperties->pszValue);
					}
					::CoTaskMemFree(pPhrase);
				}
			}
			break;
		}

		m_pSpeechContext->GetEvents(1, &curEvent, &fetched);
	}

	return hr;
}

void Person2::PerformAction(LPCWSTR pszSpeechTag)
{
	wstring wtag = wstring(pszSpeechTag);
	string tag = string(wtag.begin(), wtag.end());
	fprintf(stdout, "%s\n", tag.c_str());
	Game::Instance()->PrintText(tag);

	if (tag == "left")
	{
		float dist = 3.0f;
		glm::vec3 pos = hands[0].pos + (hands[0].look * dist);
		FireProjectile(pos, hands[0].look);
	}

	if (tag == "right")
	{
		float dist = 3.0f;
		glm::vec3 pos = hands[1].pos + (hands[1].look * dist);
		FireProjectile(pos, hands[1].look);
	}

	if (tag == "car")
	{
		VRGame2 * game = (VRGame2 *)Game::Instance();
		glm::vec3 point;
		bool hit = game->ground->rayIntersectsWorldPlane(game->camera->transform->position, game->camera->transform->look, point);
		if (hit)
		{
			point.y = 10;
			game->physicsFactory->CreateVehicle(point);
			game->soundSystem->PlaySound("spawn", point);
		}
	}
	if (tag == "model")
	{
		VRGame2 * game = (VRGame2 *)Game::Instance();
		glm::vec3 point;
		bool hit = game->ground->rayIntersectsWorldPlane(game->camera->transform->position, game->camera->transform->look, point);
		if (hit)
		{
			point.y = 5;
			game->physicsFactory->CreateRandomObject(point, glm::quat());
			game->soundSystem->PlaySound("spawn", point);
		}
	}

	if (tag == "coriolis")
	{
		VRGame2 * game = (VRGame2 *)Game::Instance();
		glm::vec3 point;
		bool hit = game->ground->rayIntersectsWorldPlane(game->camera->transform->position, game->camera->transform->look, point);
		if (hit)
		{
			point.y = 5;
			game->physicsFactory->CreateFromModel("coriolis", point, glm::quat(), glm::vec3(3, 3, 3));
			game->soundSystem->PlaySound("spawn", point);
		}
	}

	if (tag == "head camera off")
	{
		headCamera = false;
	}

	if (tag == "head camera on")
	{
		headCamera = true;
	}

	if (tag == "reset scene")
	{
		VRGame2 * game = (VRGame2 *)Game::Instance();
		game->ResetScene();
	}
}

void Person2::FireProjectile(glm::vec3 pos, glm::vec3 look)
{
	VRGame2 * game = (VRGame2 *)Game::Instance();

	glm::quat q(RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat());
	glm::normalize(q);
	shared_ptr<PhysicsController> physicsComponent = game->physicsFactory->CreateSphere(1, pos, q);
	game->soundSystem->PlaySound("Fire", pos);
	//soundSystem->Vibrate(200, 1.0f);
	float force = 3000.0f;
	physicsComponent->rigidBody->applyCentralForce(GLToBtVector(look) * force);
}
