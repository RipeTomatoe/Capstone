#ifndef __PORTAL_H_
#define __PORTAL_H_

#include "Renderable.h"
#include "DirectXRenderer.h"

class PortalPhantom : public hkpPhantomCallbackShape
{
public:
	bool left;
	PortalPhantom(){left=true;}
	virtual void phantomEnterEvent(const hkpCollidable* collidableA, const hkpCollidable* collidableB, const hkpCollisionInput& env);
	virtual void phantomLeaveEvent(const hkpCollidable* collidableA, const hkpCollidable* collidableB)
	{
		left = true;
	}
};

class Portal : public Renderable
{
	friend class PortalPhantom;
	friend class DirectXRenderer;
private:
	static LPDIRECT3DVERTEXBUFFER9	pVB;
	static LPDIRECT3DTEXTURE9		pTexture[2];
	static FMOD::Sound*				sPortEnter[2];
	LPDIRECT3DTEXTURE9				pTex;
	Vector							vRotation;
	Vector							vScale;
	hkpRigidBody*					pPhantom;
public:
	PortalPhantom*					pEvents;
	Vector							vPosition;
	Vector							vNormal;
	FMOD::Channel*					sChannel;
	static Portal*					aPortals[2];
	static void reset(void)
	{
		if(aPortals[0] != NULL)
		{
			aPortals[0]->setToDie();
			aPortals[0] = NULL;
		}
		if(aPortals[1] != NULL)
		{
			aPortals[1]->setToDie();
			aPortals[1] = NULL;
		}
	}
	void Render(void);
	void Update(double dTime);
	Portal(Vector position, Vector normal);
	void SetUserData(hkUlong data){if(pPhantom!=NULL) pPhantom->setUserData(data);pTex = pTexture[data];};
	virtual ~Portal(void);
};

#endif