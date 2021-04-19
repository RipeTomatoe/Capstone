#ifndef __SOUNDENGINE_H_
#define __SOUNDENGINE_H_

#include "FMOD\Win32\fmod.hpp"
#include "FMOD\Win32\fmod_errors.h"
#include <stdio.h>
#include "LogManager.h"
#include "Destroyer.h"

class SoundEngine
{
	friend class Destroyer<SoundEngine>;
public:
	static SoundEngine* getInstance(void)
	{
		if(pInstance == 0)
		{
			pInstance = new SoundEngine();
			destroyer.setSingleton(pInstance);
		}
		return pInstance;
	};
	void createSoundEffect(const char* fileName, FMOD::Sound** sound, FMOD_MODE flags=FMOD_SOFTWARE, bool loop=false)
	{
		char temp[255];
		sprintf_s(temp, "Resources/Sounds/%s", fileName);

		result = pSystem->createSound(temp, flags, NULL, sound);
		SOUND_ERRCHECK(result);

		if(loop==false)
			result = (*sound)->setMode(FMOD_LOOP_OFF);
		else if(loop==true)
			result = (*sound)->setMode(FMOD_LOOP_NORMAL);
		SOUND_ERRCHECK(result);
	}
	void playSoundEffect(FMOD::Sound* sound, const float volume=0.3f)
	{
		result = pSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &pSoundFXChannel);
		SOUND_ERRCHECK(result);

		pSoundFXChannel->setVolume(volume);
	}
	void playSoundEffect(const char* fileName, const float volume=0.3f)
	{
		char temp[255];
		sprintf_s(temp, "Resources/Sounds/%s", fileName);

		FMOD::Sound* sTemp;

		result = pSystem->createSound(temp, FMOD_SOFTWARE, NULL, &sTemp);
		SOUND_ERRCHECK(result);

		result = sTemp->setMode(FMOD_LOOP_OFF);
		SOUND_ERRCHECK(result);

		result = pSystem->playSound(FMOD_CHANNEL_FREE, sTemp, false, &pSoundFXChannel);
		SOUND_ERRCHECK(result);

		pSoundFXChannel->setVolume(volume);
	}
	void playMusic(const char* fileName, bool loop=true, const float volume=0.3f)
	{
		FMOD::Sound* music;

		char temp[255];
		sprintf_s(temp, "Resources/Sounds/%s", fileName);

		result = pSystem->createStream(temp, FMOD_SOFTWARE, NULL, &music);
		SOUND_ERRCHECK(result);

		if(loop)
			result = music->setMode(FMOD_LOOP_NORMAL);
		else
			result = music->setMode(FMOD_LOOP_OFF);
		SOUND_ERRCHECK(result);

		pMusicChannel->stop();

		result = pSystem->playSound(FMOD_CHANNEL_FREE, music, false, &pMusicChannel);
		SOUND_ERRCHECK(result);

		pMusicChannel->setVolume(volume);
	}
	FMOD::Channel* getSoundEffectChannel(void)
	{
		return pSoundFXChannel;
	}
	FMOD::System* getSoundSystem(void)
	{
		return pSystem;
	}
	void stopMusic(void) const
	{
		pMusicChannel->stop();
	}
	void stopSounds(void) const
	{
		pSoundFXChannel->stop();
	}
	void update(void)
	{
		pSystem->update();
	}
private:
	static SoundEngine* pInstance;
	static Destroyer<SoundEngine> destroyer;
	FMOD::System* pSystem;
	FMOD_RESULT result;
	FMOD::Channel* pSoundFXChannel;
	FMOD::Channel* pMusicChannel;

	void SOUND_ERRCHECK(FMOD_RESULT res);
	SoundEngine(void)
	{
		LogManager::getInstance()->trace("FMOD Initializing...");

		unsigned int version;
		int numDrivers;
		FMOD_SPEAKERMODE speakerMode;
		FMOD_CAPS caps;
		char name[256];

		//Setup Sound
		result = FMOD::System_Create(&pSystem);
		SOUND_ERRCHECK(result);

		result = pSystem->getVersion(&version);
		SOUND_ERRCHECK(result);

		if(version < FMOD_VERSION)
		{
			LogManager::getInstance()->trace("[ERROR] You are using an old version of FMOD %08x. This program requires %08x\n", version, FMOD_VERSION);
		}

		result = pSystem->getNumDrivers(&numDrivers);
		SOUND_ERRCHECK(result);

		if(numDrivers == 0)
		{
			result = pSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
			SOUND_ERRCHECK(result);
		}
		else
		{
			result = pSystem->getDriverCaps(0, &caps, 0, 0, &speakerMode);
			SOUND_ERRCHECK(result);

			result = pSystem->setSpeakerMode(speakerMode);
			SOUND_ERRCHECK(result);

			result = pSystem->getDriverInfo(0, name, 256, 0);
			SOUND_ERRCHECK(result);
		}

		result = pSystem->init(100, FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_SOFTWARE_OCCLUSION | FMOD_INIT_SOFTWARE_HRTF | FMOD_INIT_GEOMETRY_USECLOSEST, 0);
		if(result == FMOD_ERR_OUTPUT_CREATEBUFFER) //Speaker mode not supported, set to stereo
		{
			result = pSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
			SOUND_ERRCHECK(result);

			result = pSystem->init(100, FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_SOFTWARE_OCCLUSION | FMOD_INIT_SOFTWARE_HRTF | FMOD_INIT_GEOMETRY_USECLOSEST, 0);
			SOUND_ERRCHECK(result);
		}

		result = pSystem->set3DSettings(1.0, 1.0f, 1.f);
		SOUND_ERRCHECK(result);

		LogManager::getInstance()->trace("FMOD successfully initialized.");
	}
	virtual ~SoundEngine(void)
	{
		if(pSystem != NULL) pSystem->release();
		pSystem = NULL;
		if(pSoundFXChannel != NULL) pSoundFXChannel->stop();
		if(pMusicChannel != NULL) pMusicChannel->stop();
	}
};

#endif //__SOUNDENGINE_H_