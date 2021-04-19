#include "RenderablePhysicsMap.h"

RenderablePhysicsMap* RenderablePhysicsMap::pInstance = NULL;
Destroyer<RenderablePhysicsMap> RenderablePhysicsMap::pDestroyer;

RenderablePhysicsMap::RenderablePhysicsMap(void)
{
	index = 0;
}

RenderablePhysicsMap::~RenderablePhysicsMap(void)
{
	theMap.clear();
}