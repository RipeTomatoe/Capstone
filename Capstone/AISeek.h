#ifndef __AISEEK_H_
#define __AISEEK_H_

#include "AIBehaviour.h"

class AISeek : public AIBehaviour
{
private:
	hkpRigidBody* pSeek;
	hkpRigidBody* pSeeker;
public:
	void Update(double dTime);
	AISeek(hkpRigidBody* toSeek, hkpRigidBody* seeker);
	void setSeek(hkpRigidBody* toSeek);
	~AISeek(void);
};

#endif