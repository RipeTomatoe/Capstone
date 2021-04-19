#ifndef __AIWANDER_H_
#define __AIWANDER_H_

#include "AIBehaviour.h"

class AIWander : public AIBehaviour
{
public:
	void Update(double dTime);
	AIWander(void);
	~AIWander(void);
};

#endif