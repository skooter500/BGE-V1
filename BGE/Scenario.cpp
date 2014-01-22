#include "Scenario.h"
#include "Params.h"
#include "SteeringGame.h"
#include "Content.h"
#include "Utils.h"
#include "Sphere.h"

using namespace BGE;

Scenario::Scenario(void)
{
	leaderController = nullptr;
}

Scenario::~Scenario(void)
{

}

void Scenario::Update(float timeDelta)
{
	if (SDL_GetMouseState(NULL, NULL) && SDL_BUTTON(3))
	{
		glm::vec3 newEnemyPos = Game::Instance()->camera->transform->position + Game::Instance()->camera->transform->look * 100.0f;
		if (leaderController != nullptr)
		{
			leaderController->targetPos = newEnemyPos;
		}
	}
}