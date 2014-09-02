#include "SkeletonMapper.h"
#include "PhysicsController.h"
#include "Game.h"
#include "KinematicMotionState.h"
#include "Params.h"

BGE::SkeletonMapper::SkeletonMapper(shared_ptr<GameComponent> owner, glm::vec3 scale)
{
	this->owner = owner;
	transform.scale = scale;

	if (Params::GetBool("leapHeadMode"))
	{
		transform.orientation = glm::angleAxis(90.0f, glm::vec3(1, 0, 0));
	}
}


BGE::SkeletonMapper::~SkeletonMapper()
{
}

void BGE::SkeletonMapper::UpdateKnob(string tag, glm::vec3 pos)
{

	glm::vec3 transformedPos = transform.TransformPosition(pos, true);
	shared_ptr<PhysicsController> knobController;
	shared_ptr<GameComponent> knob = owner->FindComponentByTag(tag);
	if (knob == nullptr)
	{		
		knobController = Game::Instance()->physicsFactory->CreateSphere(1.0f, transformedPos, glm::quat(), true, false);
		knobController->rigidBody->setMotionState(new KinematicMotionState(knobController->parent));
		knobController->tag = "HandJointController";
		knobController->parent->tag = tag;
		knobController->transform->diffuse = glm::vec3(1, 0, 1);
		owner->Attach(knobController->parent);
	}
	else
	{
		knob->transform->position = transformedPos;
	}
}

void BGE::SkeletonMapper::UpdateBone(string tag, glm::vec3 start, glm::vec3 end, bool withKnobs)
{
	glm::vec3 jointPos[2];

	if (Params::GetBool("leapHeadMode"))
	{
		start.x = -start.x;
		end.x = -end.x;

		start.y = -start.y;
		end.y = -end.y;
	}

	jointPos[0] = transform.TransformPosition(start, true);
	jointPos[1] = transform.TransformPosition(end, true);

	
	glm::vec3 boneVector = jointPos[1] - jointPos[0];
	float boneLength = glm::length(boneVector);
	glm::vec3 centrePos = jointPos[0] + ((boneVector) / 2.0f);

	boneVector = glm::normalize(boneVector);
	glm::vec3 axis = glm::cross(Transform::basisUp, boneVector);
	axis = glm::normalize(axis);
	float theta = (float)glm::acos<float>(glm::dot<float>(Transform::basisUp, boneVector));
	glm::quat q = glm::angleAxis(glm::degrees(theta), axis);

	//LineDrawer::DrawLine(jointPos[0], jointPos[1], glm::vec3(0, 0, 1));
	
	//std::map<std::string, std::shared_ptr<PhysicsController>>::iterator it = mapStuff.find(tag);
	shared_ptr<GameComponent> bone = owner->FindComponentByTag(tag);
	shared_ptr<PhysicsController> boneController;
	if (bone == nullptr)
	{
		boneController = Game::Instance()->physicsFactory->CreateCylinder(0.5f, boneLength, centrePos, q, true, false);
		boneController->rigidBody->setMotionState(new KinematicMotionState(boneController));
		boneController->tag = "HandBoneController";
		boneController->parent->tag = tag;
		boneController->transform->diffuse = glm::vec3(0, 0, 1);
		mapStuff[tag] = boneController;
		owner->Attach(boneController->parent);
	}
	else
	{
		bone->transform->orientation = q;
		bone->transform->position = centrePos;
	}

	if (withKnobs)
	{
		string key[2];
		key[0] = tag + "0";
		key[1] = tag + "1";
		UpdateKnob(key[0], start);
		UpdateKnob(key[1], end);
	}
}
