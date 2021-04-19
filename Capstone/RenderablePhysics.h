#ifndef __RENDERABLEPHYSICS_H_
#define __RENDERABLEPHYSICS_H_

#include "Renderable.h"
#include "DirectXRenderer.h"

class RenderablePhysics : public Renderable
{
	friend class Camera;
protected:
	hkpRigidBody*	pBody;
	ULONG			userData;
	bool			bPortal;
public:
	virtual void Update(double dTime) = 0;
	virtual void Render(void) = 0;
	void setToDie(void){bToDie=true;}
	ULONG getUserData(void){return userData;}
	void setUserData(ULONG data){userData = data;}
	RenderablePhysics(void){bToDie=false;userData=0;bPortal=false;}
	virtual ~RenderablePhysics(void){}
};

#endif