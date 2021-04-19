#ifndef __EFFECT_H_
#define __EFFECT_H_

#include "DirectXRenderer.h"

class Effect
{
private:
	ID3DXEffect* effect;
public:
	Effect(const char* fileName);
	ID3DXEffect* getEffect(void) { return effect; }
	virtual ~Effect(void) { SAFE_RELEASE(effect); }
};

#endif //__EFFECT_H_