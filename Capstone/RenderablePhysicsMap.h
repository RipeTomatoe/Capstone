#ifndef __RENDERABLEPHYSICSMAP_H_
#define __RENDERABLEPHYSICSMAP_H_

#include "DirectXRenderer.h"

class RenderablePhysicsMap
{
	friend class Destroyer<RenderablePhysicsMap>;
public:
	typedef std::map<hkUlong, RenderablePhysics*> MapType;
private:
	static RenderablePhysicsMap* pInstance;
	static Destroyer<RenderablePhysicsMap> pDestroyer;
	hkUlong index;
	RenderablePhysicsMap(void);
	~RenderablePhysicsMap(void);
public:
	static inline RenderablePhysicsMap* getInstance(void)
	{
		if(pInstance == NULL)
		{
			pInstance = new RenderablePhysicsMap();
			pDestroyer.setSingleton(pInstance);
		}
		return pInstance;
	}
	void incrementIndex(void){ index++; }
	hkUlong getIndex(void) const {return index;}
	MapType theMap;
};

#endif