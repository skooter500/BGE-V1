#pragma once
#include "GameComponent.h"
#include "PhysicsController.h"

using namespace std;

namespace BGE
{
	class SkeletonMapper
	{
	public:
		BGE::SkeletonMapper::SkeletonMapper(shared_ptr<GameComponent> owner, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
		~SkeletonMapper();

		shared_ptr<GameComponent> owner;

		std::map<std::string, std::shared_ptr<PhysicsController>> mapStuff;

		Transform transform;

		void UpdateBone(string tag, glm::vec3 start, glm::vec3 end, bool withKnobs = true);
		void UpdateKnob(string tag, glm::vec3 start);

	};
}
