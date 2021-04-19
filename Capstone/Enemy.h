#ifndef __ENEMY_H_
#define __ENEMY_H_

#include "SceneNode.h"
#include "AIWander.h"
#include "AISeek.h"

class Enemy : public SceneNode
{
public:
	enum SHAPE_TYPE
	{
		CAPSULE = 0,
		CUBE,
		SPHERE
	};
private:
	AIBehaviour* pBehaviour;
	hkpCharacterRigidBody* pBody;
	hkpCharacterContext* pBodyContext;
public:
	void Update(double dTime);
	void Render(void);
	Enemy(const char* fileName, float x = 0.f, float y = 0.f, float z = 0.f, SHAPE_TYPE type = CAPSULE);
	~Enemy(void);
};

#endif //__ENEMY_H_