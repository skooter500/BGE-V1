#include "SoundSystem.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include "Exception.h"
#include "Content.h"
#include "Utils.h"
using namespace BGE;

void CheckFMODResult( FMOD_RESULT res )
{
	if (res != FMOD_OK)
	{
		const char * error = FMOD_ErrorString(res);
		throw BGE::Exception(error);
	}
}

// Doesnt work... so dont call it
void SoundSystem::Vibrate(int millis, float power)
{
	SDL_Haptic *haptic;

	// Open the device
	SDL_Joystick * joy;
	joy = SDL_JoystickOpen(0);
	haptic = SDL_HapticOpenFromJoystick( joy );
	if (haptic == NULL)
	{
		return;
	}
	
	if (SDL_HapticRumbleInit( haptic ) != 0)
	{
		return;
	}

	if (SDL_HapticRumblePlay( haptic, power, millis ) != 0)
	{
		return;
	}

	// Clean up
	SDL_JoystickClose(joy);
	SDL_HapticClose( haptic );
}

SoundSystem::SoundSystem(void)
{
	enabled = true;
}


SoundSystem::~SoundSystem(void)
{ 
}

void SoundSystem::Initialise()
{
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS caps;
	CheckFMODResult(FMOD::System_Create(& fmodSystem));
	CheckFMODResult(fmodSystem->getDriverCaps(0, &caps, 0, &speakerMode));
	CheckFMODResult(fmodSystem->setSpeakerMode(speakerMode));

	CheckFMODResult(fmodSystem->init(1000, FMOD_INIT_NORMAL, 0));
}

void SoundSystem::PlayHitSoundIfReady(GameComponent * object, int interval)
{
	if (! enabled)
	{
		return;
	}
	long now = SDL_GetTicks();
	// Has this object already played a sound?
	map<GameComponent *, SoundEvent>::iterator it = soundEvents.find(object);
	SoundEvent soundEvent;
	bool isPlaying = false;
	if (it != soundEvents.end())
	{
		if (now - it->second.last > interval)
		{
			// Is the sound already playing?
			soundEvent = it->second;
			soundEvent.channel->isPlaying(& isPlaying);
			if (isPlaying)
			{
				return;
			}
		}
		else
		{
			// Its too soon to play this object's sound
			return;
		}
	}
	else
	{
		int which = rand() % NUM_HIT_SOUNDS;
		stringstream ss;
		ss << "Hit" << which;
		soundEvent.sound = Content::LoadSound(ss.str());
	}

	fmodSystem->playSound(FMOD_CHANNEL_FREE, soundEvent.sound, false, & soundEvent.channel);
	if (soundEvent.channel != NULL)
	{
		if (isPlaying)
		{
			soundEvent.channel->set3DAttributes(& GLToFMODVector(object->transform->position), & GLToFMODVector(glm::vec3(0)));
		}
	}
	soundEvent.last = now;
	soundEvents[object] = soundEvent;
}

void BGE::SoundSystem::Update()
{
	if (! enabled)
	{
		return;
	}
	shared_ptr<Camera> camera = Game::Instance()->camera;
	fmodSystem->set3DListenerAttributes(0,
		& GLToFMODVector(camera->transform->position)
		, & GLToFMODVector(glm::vec3(0))
		, & GLToFMODVector(camera->transform->look)
		, & GLToFMODVector(camera->transform->up)
		);
	fmodSystem->update();

}

void SoundSystem::PlaySound(string name, glm::vec3 pos)
{
	if (! enabled)
	{
		return;
	}
	FMOD::Sound * sound = Content::LoadSound(name);
	FMOD::Channel * channel;
	fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, & channel);
	if (channel != NULL)
	{
		channel->set3DAttributes(& GLToFMODVector(pos), & GLToFMODVector(glm::vec3(0)));	
	}
}


