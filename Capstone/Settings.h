#ifndef __SETTINGS_H_
#define __SETTINGS_H_

#include "DirectXRenderer.h"
#include "Destroyer.h"

class Settings
{
	friend class Destroyer<Settings>;
public:
	static Settings* getInstance(void)
	{
		if(pInstance == NULL)
		{
			pInstance = new Settings();
			destroyer.setSingleton(pInstance);
		}
		return pInstance;
	}
private:
	static Settings* pInstance;
	static Destroyer<Settings> destroyer;
	UINT16 width, height;
	BYTE volume;
	bool fullScreen,sound;
	Settings(void)
	{

	}
	~Settings(void)
	{

	}
};

#endif //__SETTINGS_H_