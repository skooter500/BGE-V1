#include "LeapHands.h"
#include "Utils.h"
#include "LineDrawer.h"
#include "PhysicsController.h"
#include "Game.h"
#include "KinematicMotionState.h"

using namespace Leap;

BGE::LeapHands::LeapHands() :GameComponent(true)
{
}


BGE::LeapHands::~LeapHands()
{
}

bool BGE::LeapHands::Initialise()
{
	mapper = make_shared<SkeletonMapper>(This(), glm::vec3(0.2f, 0.2f, 0.2f));
	
	return GameComponent::Initialise();
}

void BGE::LeapHands::Update(float timeDelta)
{
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
				ss << "Hand: " << handId << "Finger: " << fingerId << " Bone: " << b;
				Game::Instance()->PrintText(ss.str());
				mapper->UpdateBone(ss.str(), LeapToGlVec3(bone.prevJoint()), LeapToGlVec3(bone.nextJoint()), true);
			}	
			// Draw some other bits of the hand
			if (!firstFinger)
			{
				for (int b = 0; b < 2; ++b)
				{
					stringstream ss;
					ss << "Hand: " << handId << "Finger: " << (fingerId - 1) << "Finger: " << (fingerId) << " Bone: " << b;
					Game::Instance()->PrintText(ss.str());
					Bone startBone = previousFinger.bone(static_cast<Bone::Type>(b));
					Bone endBone = finger.bone(static_cast<Bone::Type>(b));
					if ((b == 1) && (fingerId == 1))
					{
						mapper->UpdateBone(ss.str(), LeapToGlVec3(startBone.nextJoint()), LeapToGlVec3(endBone.prevJoint()), false);
					}
					else
					{
						mapper->UpdateBone(ss.str(), LeapToGlVec3(startBone.prevJoint()), LeapToGlVec3(endBone.prevJoint()), false);
					}
				}
			}
			previousFinger = finger;
			firstFinger = false;
			++fingerId;
		}
		++handId;
	}
	// There are no tracked hands, so remove them
	if (handId == 0)
	{
		ClearAllChildren();
	}

	GameComponent::Update(timeDelta);
}

void BGE::LeapHands::Draw()
{
	GameComponent::Draw();
}

void BGE::LeapHands::Cleanup()
{
	GameComponent::Cleanup();
}