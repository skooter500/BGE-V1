#include "XBoxController.h"
#include "Game.h"
#include <limits>

using namespace BGE;

XBoxController::XBoxController(bool hasTransform) :GameComponent(hasTransform)
{
	disablePitch = false;
}

XBoxController::~XBoxController(void)
{
}

bool XBoxController::Initialise()
{
	// Initialize the joystick subsystem
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	return GameComponent::Initialise();
}


void CheckOverflow( int & x )
{
	if (x == -32768)
	{
		x = - x;
	}
}

void XBoxController::Update(float timeDelta)
{
	SDL_Joystick *joy;
	if (SDL_NumJoysticks() > 0) {
		// Open joystick
		joy = SDL_JoystickOpen(0);
		if (joy) {
			stringstream ss;
			ss << SDL_JoystickNameForIndex(0) << " detected";
			Game::Instance()->PrintText(ss.str());

			float range = 1;

			int strafeAxis = SDL_JoystickGetAxis(joy, 0);
			
			if (glm::abs<int>(strafeAxis) > 8000)
			{
				float strafe = ((float) strafeAxis / (float) numeric_limits<short int>::max()) * range;
				transform->Strafe(strafe);
			}
			
			int walkAxis = SDL_JoystickGetAxis(joy, 1);
			CheckOverflow(walkAxis);
			if (glm::abs<int>(walkAxis) > 8000)
			{
				float walk = ((float) walkAxis / (float) numeric_limits<short int>::max()) * range;
				
				transform->Walk(-walk);
			}

			range = 3; 
			int x = SDL_JoystickGetAxis(joy, 2);	
			float yaw = ((float) x / (float) numeric_limits<short int>::max()) * range;
			transform->Yaw((int) -yaw);


			int flyAxis = SDL_JoystickGetAxis(joy, 3);
			CheckOverflow(flyAxis);
			if (glm::abs<int>(flyAxis) > 8000)
			{
				float fly = ((float)flyAxis / (float)numeric_limits<short int>::max()) * (range / 2.0f);

				transform->Fly(fly);
			}

			if (!disablePitch)
			{
				int y = SDL_JoystickGetAxis(joy, 3);	
				CheckOverflow(y);
				float pitch = ((float) y / (float) numeric_limits<short int>::max()) * range;
				transform->Pitch((int) -pitch);
			}
		} else {
			Game::Instance()->PrintText("Could not get controller!!");
		}

		// Close if opened
		if (SDL_JoystickGetAttached(joy)) {
			SDL_JoystickClose(joy);
		}
	}
	else
	{
		Game::Instance()->PrintText("No game controller detected");
	}

	stringstream ss;
	ss << "Gamepad Pos: " << transform->position.x << " " << transform->position.y << " " << transform->position.z;
	Game::Instance()->PrintText(ss.str());

	ss.str("");
	ss << "Gamepad Look: " << transform->look.x << " " << transform->look.y << " " << transform->look.z;
	Game::Instance()->PrintText(ss.str());

	GameComponent::Update(timeDelta);
}
