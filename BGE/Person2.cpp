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
		return false;
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
		fprintf(stdout, "Got here4");
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
		throw BGE::Exception("No ready Kinect found!");
	}

	if (FAILED(hr))
	{
		throw BGE::Exception("Failed opening an audio stream!");
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
		throw BGE::Exception("Could not create speech recognizer. Please ensure that Microsoft Speech SDK and other sample requirements are installed.");
	}

	hr = LoadSpeechGrammar();

	if (FAILED(hr))
	{
		throw BGE::Exception("Could not load speech grammar. Please ensure that grammar configuration file was properly deployed.");
	}

	hr = StartSpeechRecognition();

	if (FAILED(hr))
	{
		throw BGE::Exception("Could not start recognizing speech.");
	}

	m_bSpeechActive = true;

	SafeRelease(pAudioBeamList);
	SafeRelease(pAudioSource);
	return GameComponent::Initialise();
}

void BGE::Person2::UpdateBone(int body, const Joint* pJoints, JointType joint0, JointType joint1)
{
	string jointKey[2];
	stringstream ss;
	ss << "Body: " << body << " Joint: " << joint0;
	jointKey[0] = ss.str();
	ss.str("");

	ss << "Body: " << body << " Joint: " << joint1;
	jointKey[1] = ss.str();
	ss.str("");

	string boneKey = jointKey[0] + "," + jointKey[1];

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

	glm::vec3 jointPos[2];
	jointPos[0] = Scale(KinectToGLVector(pJoints[joint0].Position));
	jointPos[1] = Scale(KinectToGLVector(pJoints[joint1].Position));
	
	
	glm::vec3 boneVector = jointPos[1] - jointPos[0];
	float boneLength = glm::length(boneVector) * 0.8f;
	glm::vec3 centrePos = jointPos[0] + ((boneVector) / 2.0f);
	
	boneVector = glm::normalize(boneVector);
	glm::vec3 axis = glm::cross(Transform::basisUp, boneVector);
	axis = glm::normalize(axis);
	float theta = (float) glm::acos<float>(glm::dot<float>(Transform::basisUp, boneVector));
	glm::quat q = glm::angleAxis(glm::degrees(theta), axis);

	shared_ptr<PhysicsController> boneController;
	shared_ptr<PhysicsController> jointController[2];

	map<string, shared_ptr<PhysicsController>>::iterator it = boneComponents.find(boneKey);
	if (it == boneComponents.end())
	{
		boneController = Game::Instance()->physicsFactory->CreateCylinder(0.5f, boneLength, centrePos, transform->orientation);
		boneController->rigidBody->setCollisionFlags(boneController->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		boneController->rigidBody->setActivationState(DISABLE_DEACTIVATION);
		boneController->rigidBody->setMotionState(new KinematicMotionState(boneController->parent));
		boneController->tag = "PersonBoneController";
		boneController->parent->tag = "PersonBone";
		boneController->transform->diffuse = glm::vec3(0, 0, 1);
		boneComponents[boneKey] = boneController;
	}
	else
	{
		boneController = it->second;
		boneController->transform->orientation = q;
		boneController->transform->position = centrePos;
	}

	for (int i = 0; i < 2; i++)
	{
		map<string, shared_ptr<PhysicsController>>::iterator it = boneComponents.find(jointKey[i]);
		if (it == boneComponents.end())
		{
			jointController[i] = Game::Instance()->physicsFactory->CreateSphere(1.0f, jointPos[0], transform->orientation);
			jointController[i]->rigidBody->setCollisionFlags(jointController[i]->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			jointController[i]->rigidBody->setActivationState(DISABLE_DEACTIVATION);
			jointController[i]->rigidBody->setMotionState(new KinematicMotionState(jointController[i]->parent));
			jointController[i]->tag = "PersonJointController";
			jointController[i]->parent->tag = "Personjoint";
			jointController[i]->transform->diffuse = glm::vec3(1, 0, 1);
			boneComponents[jointKey[i]] = jointController[i];
		}
		else
		{
			jointController[i] = it->second;
			jointController[i]->transform->position = jointPos[i];
		}
	}
}

void BGE::Person2::UpdateHead(int body, const Joint* pJoints, JointType joint0)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;

	// If we can't find either of these joints, exit
	if (joint0State == TrackingState_NotTracked)
	{
		return;
	}

	glm::vec3 start = Scale(KinectToGLVector(pJoints[joint0].Position));

	stringstream ss;
	ss << body << "Head" << joint0;
	string boneKey = ss.str();

	shared_ptr<Game> game = Game::Instance();

	map<string, shared_ptr<PhysicsController>>::iterator it = boneComponents.find(boneKey);
	shared_ptr<PhysicsController> boxController;
	glm::quat q = glm::angleAxis(glm::degrees(glm::half_pi<float>()), glm::vec3(1, 0, 0));
	if (it == boneComponents.end())
	{
		
		boxController = game->physicsFactory->CreateCylinder(2.0f, 0.5f, start, q);
		boxController->rigidBody->setCollisionFlags(boxController->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		boxController->rigidBody->setActivationState(DISABLE_DEACTIVATION);
		boxController->rigidBody->setMotionState(new KinematicMotionState(boxController->parent));
		boxController->tag = "PersonHeadController";
		boxController->parent->tag = "PersonHead";
		boxController->transform->diffuse = glm::vec3(0, 0, 1);
		boneComponents[boneKey] = boxController;
	}
	else
	{
		boxController = it->second;
	}

	if (headCamera)
	{
		game->camera->transform->position = start + glm::vec3(0, scale * 0.1f, 0);
		boxController->transform->position = glm::vec3(100, -100, 100);
		boxController->transform->orientation = q;
	}
	else
	{
		boxController->transform->position = this->transform->position + start;
		boxController->transform->orientation = q;
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

	if (hands[0].state == HandState::HandState_Closed)
	{
		Game::Instance()->PrintText("Left hand closed");
	}
	else
	{
		Game::Instance()->PrintText("Left hand open");
	}

	hands[1].pos = Scale(KinectToGLVector(joints[JointType_HandRight].Position));
	hands[1].state = rightHandState;
	hands[1].look = glm::normalize(KinectToGLVector(joints[JointType_HandRight].Position) - KinectToGLVector(joints[JointType_WristRight].Position));

	if (hands[1].state == HandState::HandState_Closed)
	{
		Game::Instance()->PrintText("Right hand closed");
	}
	else
	{
		Game::Instance()->PrintText("Right hand open");
	}

}

void Person2::ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies)
{	
	if (m_pCoordinateMapper && (nBodyCount > 0))
	{
		// Only track the first skeleton
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

						UpdateBone(i, joints, JointType_Head, JointType_Neck);
						UpdateBone(i, joints, JointType_Neck, JointType_SpineShoulder);
						UpdateBone(i, joints, JointType_SpineShoulder, JointType_SpineMid);
						UpdateBone(i, joints, JointType_SpineMid, JointType_SpineBase);
						UpdateBone(i, joints, JointType_SpineShoulder, JointType_ShoulderRight);
						UpdateBone(i, joints, JointType_SpineShoulder, JointType_ShoulderLeft);
						UpdateBone(i, joints, JointType_SpineBase, JointType_HipRight);
						UpdateBone(i, joints, JointType_SpineBase, JointType_HipLeft);

						// Right Arm    
						UpdateBone(i, joints, JointType_ShoulderRight, JointType_ElbowRight);
						UpdateBone(i, joints, JointType_ElbowRight, JointType_WristRight);
						UpdateBone(i, joints, JointType_WristRight, JointType_HandRight);
						UpdateBone(i, joints, JointType_HandRight, JointType_HandTipRight);
						//UpdateBone(joints, JointType_WristRight, JointType_ThumbRight);

						// Left Arm
						UpdateBone(i, joints, JointType_ShoulderLeft, JointType_ElbowLeft);
						UpdateBone(i, joints, JointType_ElbowLeft, JointType_WristLeft);
						UpdateBone(i, joints, JointType_WristLeft, JointType_HandLeft);
						UpdateBone(i, joints, JointType_HandLeft, JointType_HandTipLeft);
						//UpdateBone(joints, JointType_WristLeft, JointType_ThumbLeft);

						// Right Leg
						UpdateBone(i, joints, JointType_HipRight, JointType_KneeRight);
						UpdateBone(i, joints, JointType_KneeRight, JointType_AnkleRight);
						UpdateBone(i, joints, JointType_AnkleRight, JointType_FootRight);

						// Left Leg
						UpdateBone(i, joints, JointType_HipLeft, JointType_KneeLeft);
						UpdateBone(i, joints, JointType_KneeLeft, JointType_AnkleLeft);
						UpdateBone(i, joints, JointType_AnkleLeft, JointType_FootLeft);

						UpdateHead(i, joints, JointType_Head);
						//break;
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
			else
			{
				throw BGE::Exception("Could not create speech context");
			}
		}
		else
		{
			throw BGE::Exception("Acoustic models not installed");
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
		shared_ptr<Game> game = Game::Instance();
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
		shared_ptr<Game> game = Game::Instance();
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
		shared_ptr<Game> game = Game::Instance();
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
		shared_ptr<VRGame2> game = dynamic_pointer_cast<VRGame2>(Game::Instance());
		game->ResetScene();
	}
}

void Person2::FireProjectile(glm::vec3 pos, glm::vec3 look)
{
	shared_ptr<Game> game = Game::Instance();

	glm::quat q(RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat());
	glm::normalize(q);
	shared_ptr<PhysicsController> physicsComponent = game->physicsFactory->CreateSphere(1, pos, q);
	game->soundSystem->PlaySound("Fire", pos);
	//soundSystem->Vibrate(200, 1.0f);
	float force = 3000.0f;
	physicsComponent->rigidBody->applyCentralForce(GLToBtVector(look) * force);
}
