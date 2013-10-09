#include "FPSController.h"
#include "Game.h"

using namespace BGE;

FPSController::FPSController(void)
{
	position = glm::vec3(0.0f, 10.0f, 10.0f);
	worldMode = world_modes::to_parent;
}

bool FPSController::Initialise()
{
	UpdateFromParent();
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
		Walk(moveSpeed * timeDelta);
	}

	if (keyState[SDL_SCANCODE_S])
	{
		Walk(-moveSpeed * timeDelta);
	}

	if (keyState[SDL_SCANCODE_A])
	{
		Strafe(-moveSpeed * timeDelta);
	}

	if (keyState[SDL_SCANCODE_D])
	{
		Strafe(moveSpeed * timeDelta);
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
		Yaw(yaw * scale);
	}
	if (pitch != 0)
	{
		Pitch(pitch * scale);
	}
	SDL_WarpMouseInWindow(
		Game::Instance()->GetMainWindow()
		,midX
		,midY
		);
	GameComponent::Update(timeDelta);
	//Controller::Update(this, this->parent);
}