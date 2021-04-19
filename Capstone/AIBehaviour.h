#ifndef __AIBEHAVIOUR_H_
#define __AIBEHAVIOUR_H_

#include "DirectXRenderer.h"
#include "RandomNumber.h"

class AIBehaviour
{
protected:
	float	fDirection;			//Direction angle in radians
	float	fDestination;		//Destination angle in radians
	float	fRotationSpeed;		//Turning speed (This value should be around 1/120, or smaller)
	float	fDeadzone;			//The give or take when reaching next direction
	double	dTimeShift;			//Amount of time before we find a new point.
	double	dTimeElapsed;		//Time elapsed;
	AIBehaviour(void){	fDirection = fDestination = 0.f;
						dTimeElapsed = 0.0;
						fRotationSpeed = 1.f/120.f;
						fDeadzone = 10.f * HK_REAL_DEG_TO_RAD;
						dTimeShift = 3.0;}
	virtual ~AIBehaviour(void){}
public:
	virtual void Update(double dTime) = 0;
	float getAngle(void){return fDirection;}
	hkVector4 getHKVector4Forward(void){ return hkVector4(sin(fDirection), 0, (-cos(fDirection))); }
	Vector getForward(void){ return Vector(sin(fDirection), 0, (-cos(fDirection))); }
	void setTimeShift(double newTime){dTimeShift = newTime;}
};

#endif