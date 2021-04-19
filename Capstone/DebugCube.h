#ifndef __DEBUGCUBE_H_
#define __DEBUGCUBE_H_

#include "RenderablePhysics.h"
#include "DirectXRenderer.h"
#include "RenderablePhysicsMap.h"

class DebugCube : public RenderablePhysics
{
	friend class DirectXRenderer;
public:
	enum MOVEMENT_TYPE
	{
		DYNAMIC = 0x1,
		STATIC = 0x0
	};
private:
	static LPDIRECT3DVERTEXBUFFER9	pVB;
	static LPDIRECT3DTEXTURE9		pDefault;
	LPDIRECT3DTEXTURE9				pTexture;
	LPDIRECT3DTEXTURE9				pBump;
	hkpRigidBody*					pBody;
	float							fU, fV;
	Vector							vecScale;
	Vector							vecPosition;
	Vector							vecRotation;
public:
	void Render(void);
	void Update(double dTime);
	void SetUTiling(float u);
	void SetVTiling(float v);
	void SetUVTiling(float u, float v);
	void SetTexture(const char* fileName);
	void SetBump(const char* fileName);
	void SetPortalable(bool port){bPortal=port;};
	DebugCube(Vector position, float width, float length, float height, MOVEMENT_TYPE type = STATIC, float mass = 10.f);
	~DebugCube(void);
};

#endif //__DEBUGCUBE_H_