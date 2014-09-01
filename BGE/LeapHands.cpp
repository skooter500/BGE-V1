#include "LeapHands.h"
#include "Utils.h"
#include "LineDrawer.h"
#include "PhysicsController.h"
#include "Game.h"
#include "KinematicMotionState.h"

using namespace Leap;
using namespace BGE;

struct BoneData
{
	string key;
	glm::vec3 pos[2];
	bool knobs;
	BoneData(string key, glm::vec3 start, glm::vec3 end, bool knobs)
	{
		this->key = key;
		pos[0] = start;
		pos[1] = end;
		this->knobs = knobs;
	}
	BoneData()
	{

	}
};

std::map<string, BoneData> boneData;

const int boneDataPerHand = 28;

BGE::LeapHands::LeapHands() :GameComponent(true)
{
	trackedHands = 0;
	InitializeCriticalSection(& criticalSection);
}


BGE::LeapHands::~LeapHands()
{
	DeleteCriticalSection(& criticalSection);
}

bool BGE::LeapHands::Initialise()
{
	if (headMode)
	{
		controller.config().setBool("head_mounted_display_mode", true);
	}

	controller.addListener(* this);

	mapper = make_shared<SkeletonMapper>(This(), glm::vec3(0.1f, 0.1f, 0.1f));
	
	return GameComponent::Initialise();
}

void BGE::LeapHands::Update(float timeDelta)
{	
	list<shared_ptr<GameComponent>>::iterator cit = children.begin();
	while (cit != children.end())
	{
		(*cit ++)->transform->position = glm::vec3(0, -10, 0);
	}

	EnterCriticalSection(&criticalSection);
	map<string, BoneData>::iterator it = boneData.begin();
	int currentBone = 0;
	if (trackedHands > 0)
	{
		while (it != boneData.end())
		{
			BoneData bone = it->second;
			mapper->UpdateBone(bone.key, bone.pos[0], bone.pos[1], bone.knobs);
			++it;
			++currentBone;
			if ((trackedHands == 1) && (currentBone == boneDataPerHand))
			{
				break;
			}
		}
	}
	LeaveCriticalSection(&criticalSection);

	GameComponent::Update(timeDelta);
}

void BGE::LeapHands::Draw()
{
	GameComponent::Draw();
}

void LeapHands::Cleanup()
{
	GameComponent::Cleanup();
}

void LeapHands::onInit(const Controller& controller) 
{
	std::cout << "Initialized" << std::endl;
}

void LeapHands::onConnect(const Controller& controller) 
{
	std::cout << "Connected" << std::endl;
	/*controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_SWIPE);*/
}

void LeapHands::onDisconnect(const Controller& controller) 
{
	// Note: not dispatched when running in a debugger.
	std::cout << "Disconnected" << std::endl;
}

void LeapHands::onExit(const Controller& controller) 
{
	std::cout << "Exited" << std::endl;
}

void LeapHands::onFocusGained(const Controller& controller) {
	std::cout << "Focus Gained" << std::endl;
}

void LeapHands::onFocusLost(const Controller& controller) {
	std::cout << "Focus Lost" << std::endl;
}

void LeapHands::onDeviceChange(const Controller& controller) {
	std::cout << "Device Changed" << std::endl;
	const DeviceList devices = controller.devices();

	for (int i = 0; i < devices.count(); ++i) {
		std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
	}
}

void LeapHands::onServiceConnect(const Controller& controller) {
	std::cout << "Service Connected" << std::endl;
}

void LeapHands::onServiceDisconnect(const Controller& controller) {
	std::cout << "Service Disconnected" << std::endl;
}

void LeapHands::onFrame(const Controller& controller) 
{
	//Game::Instance()->PrintFloat("Hand children: ", childrenMap.size());	
	std::map<string, BoneData> tempBoneData;
	HandList hands = controller.frame().hands();
	int handId = 0;
	for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
		// Get the first hand
		const Hand hand = *hl;

		// Get fingers
		const FingerList fingers = hand.fingers();
		int fingerId = 0;
		bool firstFinger = true;
		Finger previousFinger;
		for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
			const Finger finger = *fl;

			// Get finger bones
			for (int b = 0; b < 4; ++b) {
				Bone::Type boneType = static_cast<Bone::Type>(b);
				Bone bone = finger.bone(boneType);
				stringstream ss;
				ss << "Hand: " << handId << " Finger: " << fingerId << " Bone: " << b;
				//Game::Instance()->PrintText(ss.str());
				tempBoneData[ss.str()] = BoneData(ss.str(), LeapToGlVec3(bone.prevJoint()), LeapToGlVec3(bone.nextJoint()), true);
				//mapper->UpdateBone(ss.str(), LeapToGlVec3(bone.prevJoint()), LeapToGlVec3(bone.nextJoint()), true);
			}
			// Draw some other bits of the hand
			if (!firstFinger)
			{
				for (int b = 0; b < 2; ++b)
				{
					stringstream ss;
					ss << "Hand: " << handId << "Finger: " << (fingerId - 1) << "Finger: " << (fingerId) << " Bone: " << b;
					//Game::Instance()->PrintText(ss.str());
					Bone startBone = previousFinger.bone(static_cast<Bone::Type>(b));
					Bone endBone = finger.bone(static_cast<Bone::Type>(b));
					if ((b == 1) && (fingerId == 1))
					{
						tempBoneData[ss.str()] = BoneData(ss.str(), LeapToGlVec3(startBone.nextJoint()), LeapToGlVec3(endBone.prevJoint()), false);
					}
					else
					{
						tempBoneData[ss.str()] = BoneData(ss.str(), LeapToGlVec3(startBone.prevJoint()), LeapToGlVec3(endBone.prevJoint()), false);
					}
				}
			}
			previousFinger = finger;
			firstFinger = false;
			++fingerId;
		}
		++handId;
	}
	trackedHands = handId;

	EnterCriticalSection(&criticalSection);
	map<string, BoneData>::iterator it = tempBoneData.begin();
	boneData = tempBoneData;
	LeaveCriticalSection(&criticalSection);
}