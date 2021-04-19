#include "SoundEngine.h"

SoundEngine* SoundEngine::pInstance = NULL;
Destroyer<SoundEngine> SoundEngine::destroyer;

void SoundEngine::SOUND_ERRCHECK(FMOD_RESULT res)
{
	if(res != FMOD_OK)
	{
		LogManager::getInstance()->trace("[ERROR] FMOD Error: (%d) %s", res, FMOD_ErrorString(res));
	}
}