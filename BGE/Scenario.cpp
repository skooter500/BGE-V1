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
		shared_ptr<GameComponent> camController = Game::Instance()->camera->GetController();
		glm::vec3 newEnemyPos = camController->position + camController->look * 100.0f;
		if (leaderController != nullptr)
		{
			leaderController->targetPos = newEnemyPos;
		}
	}
}