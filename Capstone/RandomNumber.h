#ifndef __RANDOMNUMBER_H_
#define __RANDOMNUMBER_H_

#include <time.h>
#include <stdlib.h>
#include <math.h>

class RandomNumber
{
public:
	static double getRandomNumber(double low, double high)
	{
		delete pInstance;
		pInstance = new RandomNumber();
		return (pInstance->ran1()*(high-low))+low;
	}
	static int getRandomNumber(int low, int high)
	{
		delete pInstance;
		pInstance = new RandomNumber();
		return (int)(pInstance->ran1()*(high-low))+low;
	}
	static float getRandomNumber(float low, float high)
	{
		delete pInstance;
		pInstance = new RandomNumber();
		return (float)(pInstance->ran1()*(high-low))+low;
	}
private:
	static RandomNumber* pInstance;
	int seed;
	int ran1idnum;
	int qd2idnum;
	double ran1(void);
	inline double ranqd2(void)
	{
		unsigned long itemp;
		static unsigned long jflone = 0x3f800000;
		static unsigned long jflmsk = 0x007fffff;
		qd2idnum = 1664525L * qd2idnum + 1013904223L;
		itemp = jflone | (jflmsk & qd2idnum);
		return (double)(*(float *)&itemp)-1.0;
	}
	RandomNumber(void);
	~RandomNumber(void);
};

#endif