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
	return GameComponent::Initialise();
}



void BGE::LeapHands::Update(float timeDelta)
{
	HandList hands = controller.frame().hands();
	for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
		// Get the first hand
		const Hand hand = *hl;
		
		// Get fingers
		const FingerList fingers = hand.fingers();
		for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
			const Finger finger = *fl;
			
			// Get finger bones
			for (int b = 0; b < 4; ++b) {
				Bone::Type boneType = static_cast<Bone::Type>(b);
				Bone bone = finger.bone(boneType);
				/* std::cout << std::string(6, ' ') << boneNames[boneType]
					<< " bone, start: " << bone.prevJoint()
					<< ", end: " << bone.nextJoint()
					<< ", direction: " << bone.direction() << std::endl;				
				*/

				stringstream ss;
				ss << "Hand: " << hand.id() << " Finger: " << finger.id() << " Bone: " << b;

				UpdateBone(ss.str(), LeapToGlVec3(bone.prevJoint()), LeapToGlVec3(bone.nextJoint()));
				//LineDrawer::DrawLine(LeapToGlVec3(bone.prevJoint()), LeapToGlVec3(bone.nextJoint()), glm::vec3(0, 0, 1));
				//UpdateBone()
			}
		}
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

void BGE::LeapHands::UpdateBone(string tag, glm::vec3 start, glm::vec3 end)
{
	glm::vec3 jointPos[2];
	jointPos[0] = start;
	jointPos[1] = end;

	glm::vec3 boneVector = jointPos[1] - jointPos[0];
	float boneLength = glm::length(boneVector) * 0.8f;
	glm::vec3 centrePos = jointPos[0] + ((boneVector) / 2.0f);

	boneVector = glm::normalize(boneVector);
	glm::vec3 axis = glm::cross(Transform::basisUp, boneVector);
	axis = glm::normalize(axis);
	float theta = (float)glm::acos<float>(glm::dot<float>(Transform::basisUp, boneVector));
	glm::quat q = glm::angleAxis(glm::degrees(theta), axis);

	shared_ptr<GameComponent> bone = FindComponentByTag(tag);
	
	if (bone == nullptr)
	{
		shared_ptr<PhysicsController> boneController;
		boneController = Game::Instance()->physicsFactory->CreateCylinder(0.5f, boneLength, centrePos, transform->orientation, false);
		boneController->rigidBody->setCollisionFlags(boneController->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		boneController->rigidBody->setActivationState(DISABLE_DEACTIVATION);
		boneController->rigidBody->setMotionState(new KinematicMotionState(boneController->parent));
		boneController->tag = "BoneController";
		boneController->parent->tag = tag;
		boneController->transform->diffuse = glm::vec3(0, 0, 1);
		Attach(boneController->parent);
	}
	else
	{
		bone->transform->orientation = q;
		bone->transform->position = centrePos;
	}

	/*
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
	*/
}
