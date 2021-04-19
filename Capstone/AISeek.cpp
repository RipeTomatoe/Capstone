#include "AISeek.h"

AISeek::AISeek(hkpRigidBody* toSeek, hkpRigidBody* seeker)
{
	pSeek = toSeek;
	pSeeker = seeker;
	fRotationSpeed = 1.f/30.f;
}

void AISeek::setSeek(hkpRigidBody* toSeek)
{
	pSeek = toSeek;
}

AISeek::~AISeek(void)
{
	pSeek = NULL;
	pSeeker = NULL;
}

void AISeek::Update(double dTime)
{
	float x = pSeek->getPosition().m_quad.v[0] - pSeeker->getPosition().m_quad.v[0];
	float z = pSeek->getPosition().m_quad.v[2] - pSeeker->getPosition().m_quad.v[2];

	fDestination = atan2(z, x);
	if(fDestination < 0)
		fDestination += (360 * HK_REAL_DEG_TO_RAD);

	float minDZ = fDestination - fDeadzone;
	float maxDZ = fDestination + fDeadzone;
	if(!(fDirection < maxDZ && fDirection > minDZ))
	{
		//check to see which way is the faster turn
		if((fDirection - fDestination) > HK_REAL_PI)
			fDirection+=fRotationSpeed;
		else
			fDirection-=fRotationSpeed;
	}
}