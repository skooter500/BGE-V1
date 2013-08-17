#include "XBoxController.h"
#include "Game.h"
#include <limits>

using namespace BGE;

XBoxController::XBoxController(void)
{
	worldMode = GameComponent::world_modes::to_parent;
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
				Strafe(strafe);
			}
			
			int walkAxis = SDL_JoystickGetAxis(joy, 1);
			CheckOverflow(walkAxis);
			if (glm::abs<int>(walkAxis) > 8000)
			{
				float walk = ((float) walkAxis / (float) numeric_limits<short int>::max()) * range;
				
				Walk(-walk);
			}
			
			range = 3; 
			int x = SDL_JoystickGetAxis(joy, 2);	
			float yaw = ((float) x / (float) numeric_limits<short int>::max()) * range;
			Yaw((int) -yaw);

			if (!disablePitch)
			{
				int y = SDL_JoystickGetAxis(joy, 3);	
				CheckOverflow(y);
				float pitch = ((float) y / (float) numeric_limits<short int>::max()) * range;
				Pitch((int) -pitch);
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
	ss << "Gamepad Pos: " << position.x << " " << position.y << " " << position.z;
	Game::Instance()->PrintText(ss.str());

	ss.str("");
	ss << "Gamepad Look: " << look.x << " " << look.y << " " << look.z;
	Game::Instance()->PrintText(ss.str());

	GameComponent::Update(timeDelta);
}
