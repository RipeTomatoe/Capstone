#include "AIWander.h"

AIWander::AIWander(void)
{
	dTimeShift = 2.0;
	fRotationSpeed = 1.f/30.f;
}

AIWander::~AIWander(void)
{
}

void AIWander::Update(double dTime)
{
	dTimeElapsed += dTime;
	if(dTimeElapsed >= dTimeShift)
	{
		fDestination = RandomNumber::getRandomNumber(0.f, 360.f*HK_REAL_DEG_TO_RAD);
		dTimeElapsed = 0.0;
	}

	float minDZ = fDestination - fDeadzone;
	float maxDZ = fDestination + fDeadzone;
	if(!(fDirection < maxDZ && fDirection > minDZ))
	{
		//check to see which way is the faster turn
		if((fDestination - fDirection) > HK_REAL_PI)
			fDirection+=fRotationSpeed;
		else
			fDirection-=fRotationSpeed;
	}
}