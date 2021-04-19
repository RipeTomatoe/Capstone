#ifndef __BASKET_H_
#define __BASKET_H_

#include "Renderable.h"
#include "DebugCube.h"
#include "RenderablePhysicsMap.h"

class MyPhantomShape : public hkpPhantomCallbackShape
{
public:
	MyPhantomShape(){}

	virtual void phantomEnterEvent(const hkpCollidable* collidableA, const hkpCollidable* collidableB, const hkpCollisionInput& env)
	{
		hkpRigidBody* RBb = hkpGetRigidBody(collidableB);
		hkpRigidBody* RBa = hkpGetRigidBody(collidableA);

		hkUlong UDb = RBb->getUserData();
		hkUlong UDa = RBa->getUserData();

		if(RenderablePhysicsMap::getInstance()->theMap[UDb] != NULL)
		{
			if(UDa == RenderablePhysicsMap::getInstance()->theMap[UDb]->getUserData())
			{
				//check current level, set to next level.
				if(DirectXRenderer::getInstance()->getActiveScene()->currentLevel < 2)
				{
					DirectXRenderer::getInstance()->getActiveScene()->currentLevel++;

				}
				else
				{
					//you win!
				}
			}
		}
	}

	virtual void phantomLeaveEvent(const hkpCollidable* collidableA, const hkpCollidable* collidableB)
	{
		//do nothing
	}
};

class Basket : public Renderable
{
private:
	DebugCube*		cubes[5];
	hkpRigidBody*	phantom;
	ULONG			userData;
public:
	void Render(void);
	void Update(double dTime);
	void setTexture(const char* fileName);
	void setUserData(ULONG data);
	Basket(Vector position);
	~Basket(void);
};

#endif