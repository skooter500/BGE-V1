#pragma once
#include <fmod.hpp>
#include <vector>
#include <map>

#include "GameComponent.h"

#define NUM_HIT_SOUNDS 7

using namespace std;

namespace BGE
{

	struct SoundEvent
	{
		FMOD::Sound * sound;
		FMOD::Channel * channel;
		long last;
	};

	
	class SoundSystem
	{
	public:
		SoundSystem(void);
		~SoundSystem(void);

		void Initialise();
		void Update();
		void PlayHitSoundIfReady(GameComponent * component, int waitMs);
		void PlaySound(string name, glm::vec3);
		// FMOD
		FMOD::System * fmodSystem;
		map<GameComponent *, SoundEvent> soundEvents;
		void Vibrate(int millis, float power);

		bool enabled;
	};
}
