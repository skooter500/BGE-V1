#include "FPSController.h"
#include "Game.h"

using namespace BGE;

FPSController::FPSController(void)
{
}

bool FPSController::Initialise()
{
	transform->position = glm::vec3(0.0f, 10.0f, 10.0f);
	return GameComponent::Initialise();
}

FPSController::~FPSController(void)
{
}

void FPSController::Update(float timeDelta)
{
	const Uint8 * keyState = Game::Instance()->GetKeyState();

	float moveSpeed = speed;

	if (keyState[SDL_SCANCODE_LSHIFT])
	{
		moveSpeed *= 10.0f;
	}

	if (keyState[SDL_SCANCODE_W])
	{
		transform->Walk(moveSpeed * timeDelta);
	}

	if (keyState[SDL_SCANCODE_S])
	{
		transform->Walk(-moveSpeed * timeDelta);
	}

	if (keyState[SDL_SCANCODE_A])
	{
		transform->Strafe(-moveSpeed * timeDelta);
	}

	if (keyState[SDL_SCANCODE_D])
	{
		transform->Strafe(moveSpeed * timeDelta);
	}

	int x, y;
	int midX, midY;
	SDL_GetMouseState(&x,&y);
	midX = Game::Instance()->GetWidth() / 2;
	midY = Game::Instance()->GetHeight() / 2;
	float yaw, pitch;
	yaw = midX - x;
	pitch = midY - y;

	float scale = 0.1f;
	if (yaw != 0)
	{
		transform->Yaw(yaw * scale);
	}
	if (pitch != 0)
	{
		transform->Pitch(pitch * scale);
	}
	SDL_WarpMouseInWindow(
		Game::Instance()->GetMainWindow()
		,midX
		,midY
		);
	GameComponent::Update(timeDelta);
	//Controller::Update(this, this->parent);
}